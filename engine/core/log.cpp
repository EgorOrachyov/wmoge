/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#include "log.hpp"

#include <magic_enum.hpp>

#include "core/engine.hpp"
#include "debug/console.hpp"
#include "platform/file_system.hpp"

#include <cassert>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

namespace wmoge {

    void Log::listen(const std::shared_ptr<LogListener>& listener) {
        assert(listener);
        std::lock_guard guard(m_mutex);
        m_listeners.push_back(listener);
    }
    void Log::remove(const std::shared_ptr<LogListener>& listener) {
        assert(listener);
        std::lock_guard guard(m_mutex);
        m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener));
    }
    void Log::log(LogLevel level, std::string message, std::string file, std::string function, std::size_t line) {
        std::lock_guard guard(m_mutex);
        LogEntry        entry{std::move(message), std::move(file), std::move(function), line, level};
        for (auto& listener : m_listeners)
            listener->on_message(entry);
    }
    Log* Log::instance() {
        static auto g_log = std::make_unique<Log>();
        return g_log.get();
    }

    LogListenerStdout::LogListenerStdout(std::string name, LogLevel level)
        : m_name(std::move(name)), m_level(level) {
    }
    void LogListenerStdout::on_message(const LogEntry& entry) {
        if (entry.level == LogLevel::Never || entry.level < m_level)
            return;

        std::stringstream output;
        output << "[" << m_name << "] "
               << "[" << std::setw(8) << magic_enum::enum_name(entry.level) << "] "
               << "[" << std::setw(30) << std::filesystem::path(entry.file).filename().string() + ":" + std::to_string(entry.line) << "] "
               << entry.message << "\n";

        if (entry.level == LogLevel::Error)
            std::cerr << output.str();
        else
            std::cout << output.str();
    }

    LogListenerStream::LogListenerStream(std::string name, wmoge::LogLevel level)
        : m_name(std::move(name)), m_level(level) {
        Engine*     engine      = Engine::instance();
        FileSystem* file_system = engine->file_system();

        std::time_t       time = engine->get_time();
        std::stringstream log_file_name;

        log_file_name << "logs://log_"
                      << m_name << " "
                      << engine->get_time_formatted("%Y-%m-%d %H-%M-%S", time)
                      << ".log";

        file_system->open_file(log_file_name.str(), m_stream, std::ios::out);
    }
    void LogListenerStream::on_message(const wmoge::LogEntry& entry) {
        if (entry.level == LogLevel::Never || entry.level < m_level)
            return;

        m_stream << "[" << m_name << "] "
                 << "[" << std::setw(8) << magic_enum::enum_name(entry.level) << "] "
                 << "[" << std::setw(30) << std::filesystem::path(entry.file).filename().string() + ":" + std::to_string(entry.line) << "] "
                 << entry.message << "\n";
    }

    LogListenerConsole::LogListenerConsole(class Console* console, LogLevel level)
        : m_console(console), m_level(level) {
    }
    void LogListenerConsole::on_message(const LogEntry& entry) {
        if (entry.level == LogLevel::Never || entry.level < m_level)
            return;

        std::stringstream output;
        output << "[" << std::setw(8) << magic_enum::enum_name(entry.level) << "] "
               << "[" << std::setw(30) << std::filesystem::path(entry.file).filename().string() + ":" + std::to_string(entry.line) << "] "
               << entry.message << "\n";

        if (entry.level == LogLevel::Error)
            m_console->add_error(output.str());
        else if (entry.level == LogLevel::Warning)
            m_console->add_warning(output.str());
        else
            m_console->add_info(output.str());
    }

}// namespace wmoge

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

#pragma once

#include <cstddef>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @brief Level of a logged message
     */
    enum class LogLevel {
        Info    = 0,
        Warning = 1,
        Error   = 2,
        Never   = 4
    };

    /**
     * @class LogEntry
     * @brief Structure holding info about a single logged message
     */
    struct LogEntry {
        std::string message;
        std::string file;
        std::string function;
        std::size_t line;
        LogLevel    level;
    };

    /**
     * @class LogListener
     * @brief Listener interface which can be attached to a log
     */
    class LogListener {
    public:
        virtual ~LogListener()                         = default;
        virtual void on_message(const LogEntry& entry) = 0;
    };

    /**
     * @class Log
     * @brief Engine global logger
     */
    class Log {
    public:
        void listen(const std::shared_ptr<LogListener>& listener);
        void remove(const std::shared_ptr<LogListener>& listener);
        void log(LogLevel level, std::string message, std::string file, std::string function, std::size_t line);

        static Log* instance();
        static void provide(Log* log);

    private:
        std::vector<std::shared_ptr<LogListener>> m_listeners;
        std::recursive_mutex                      m_mutex;

        static Log* g_log;
    };

    /**
     * @class LogListenerStdout
     * @brief Log listener which dumps messages to out console
     */
    class LogListenerStdout final : public LogListener {
    public:
        LogListenerStdout(std::string name, LogLevel level);
        void on_message(const LogEntry& entry) override;

    private:
        std::string m_name;
        LogLevel    m_level;
    };

    /**
     * @class LogListenerStream
     * @brief Log listener which dumps messages to a file stream
     */
    class LogListenerStream final : public LogListener {
    public:
        LogListenerStream(std::fstream stream, std::string name, LogLevel level);
        void on_message(const LogEntry& entry) override;

        static std::fstream open_file(class FileSystem* file_system, const std::string& filename);
        static std::string  make_file_name(class Time* time, const std::string& logname);

    private:
        std::fstream m_stream;
        std::string  m_name;
        LogLevel     m_level;
    };

#define WG_LOG_MESSAGE(level, message)                                                                                \
    do {                                                                                                              \
        std::stringstream __ss;                                                                                       \
        __ss << message;                                                                                              \
        ::wmoge::Log::instance()->log(level, __ss.str(), __FILE__, __FUNCTION__, static_cast<std::size_t>(__LINE__)); \
    } while (false)

#define WG_LOG_INFO(message)    WG_LOG_MESSAGE(::wmoge::LogLevel::Info, message)
#define WG_LOG_WARNING(message) WG_LOG_MESSAGE(::wmoge::LogLevel::Warning, message)
#define WG_LOG_ERROR(message)   WG_LOG_MESSAGE(::wmoge::LogLevel::Error, message)

}// namespace wmoge
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

#include "cmd_line.hpp"

#include <iostream>
#include <sstream>

namespace wmoge {

    std::string CmdLineUtil::to_string(int argc, const char* const* argv) {
        std::stringstream s;
        for (int i = 0; i < argc; i++) {
            s << argv[i] << " ";
        }
        return s.str();
    }

    std::vector<std::string> CmdLineUtil::to_vector(int argc, const char* const* argv) {
        std::vector<std::string> v;
        v.reserve(argc);
        for (int i = 0; i < argc; i++) {
            v.emplace_back(argv[i]);
        }
        return std::move(v);
    }

    CmdLineOptions::CmdLineOptions(const std::string& name, const std::string& desc)
        : m_options(name, desc) {
    }

    void CmdLineOptions::add_int(const std::string& name, const std::string& desc, const std::string& value) {
        m_options.add_option("", cxxopts::Option(name, desc, cxxopts::value<int>()->default_value(value)));
    }

    void CmdLineOptions::add_bool(const std::string& name, const std::string& desc, const std::string& value) {
        m_options.add_option("", cxxopts::Option(name, desc, cxxopts::value<bool>()->default_value(value)));
    }

    void CmdLineOptions::add_string(const std::string& name, const std::string& desc, const std::string& value) {
        m_options.add_option("", cxxopts::Option(name, desc, cxxopts::value<std::string>()->default_value(value)));
    }

    std::optional<CmdLineParseResult> CmdLineOptions::parse(const std::vector<std::string>& args) {
        const int                argc = static_cast<int>(args.size());
        std::vector<const char*> argv;

        argv.reserve(argc);
        for (auto& arg : args) {
            argv.push_back(arg.c_str());
        }

        try {
            return CmdLineParseResult(m_options.parse(argc, argv.data()));
        } catch (const std::exception& e) {
            std::cerr << "failed to parse options due to: " << e.what();
        }

        return std::nullopt;
    }

    std::string CmdLineOptions::get_help() const {
        return m_options.help();
    }

    CmdLineParseResult::CmdLineParseResult(cxxopts::ParseResult parsed)
        : m_parsed(std::move(parsed)) {
    }

    int CmdLineParseResult::get_int(const std::string& name) {
        return m_parsed[name].as<int>();
    }

    bool CmdLineParseResult::get_bool(const std::string& name) {
        return m_parsed[name].as<bool>();
    }

    std::string CmdLineParseResult::get_string(const std::string& name) {
        return m_parsed[name].as<std::string>();
    }

    void CmdLineHookList::add(CmdLineHook hook) {
        m_storage.emplace_back(std::move(hook));
    }

    void CmdLineHookList::clear() {
        m_storage.clear();
    }

    Status CmdLineHookList::process(CmdLineParseResult& cmd_line) {
        for (auto& hook : m_storage) {
            const Status     status = hook(cmd_line);
            const StatusCode code   = status.code();

            if (code == StatusCode::ExitCode0) {
                return StatusCode::ExitCode0;
            }
            if (code == StatusCode::ExitCode1) {
                return StatusCode::ExitCode1;
            }
            if (code != StatusCode::Ok) {
                return status;
            }
        }
        return WG_OK;
    }

}// namespace wmoge
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

namespace wmoge {

    CmdLine::CmdLine() : m_options("wmoge", "wmoge engine runtime and tools") {
    }

    void CmdLine::add_int(const std::string& name, const std::string& desc, const std::string& value) {
        m_options.add_option("", cxxopts::Option(name, desc, cxxopts::value<int>()->default_value(value)));
    }
    void CmdLine::add_bool(const std::string& name, const std::string& desc, const std::string& value) {
        m_options.add_option("", cxxopts::Option(name, desc, cxxopts::value<bool>()->default_value(value)));
    }
    void CmdLine::add_string(const std::string& name, const std::string& desc, const std::string& value) {
        m_options.add_option("", cxxopts::Option(name, desc, cxxopts::value<std::string>()->default_value(value)));
    }

    bool CmdLine::parse(int argc, const char* const* argv) {
        try {
            m_parsed = m_options.parse(argc, argv);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "failed to parse options due to: " << e.what();
        }

        return false;
    }

    int CmdLine::get_int(const std::string& name) {
        return m_parsed[name].as<int>();
    }
    bool CmdLine::get_bool(const std::string& name) {
        return m_parsed[name].as<bool>();
    }
    std::string CmdLine::get_string(const std::string& name) {
        return m_parsed[name].as<std::string>();
    }

    std::string CmdLine::get_help() const {
        return m_options.help();
    }

}// namespace wmoge
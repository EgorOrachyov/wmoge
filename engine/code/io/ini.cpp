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

#include "ini.hpp"

#include <sstream>

namespace wmoge {

    Status IniFile::parse(const std::string& content) {
        std::string section_name;
        std::string line;

        std::stringstream file(content);
        while (!file.eof()) {
            std::getline(file, line);

            if (!line.empty() && line[line.length() - 1] == '\r') {
                line = line.substr(0, line.length() - 1);
            }
            if (line[0] == '[') {
                section_name = line.substr(1, line.find_last_of(']') - 1);
                continue;
            }
            if (line[0] == ';') {
                continue;
            }

            auto pos = line.find(" = ");
            if (pos != std::string::npos) {
                std::string key   = line.substr(0, pos);
                std::string value = line.substr(pos + 3);
                Var         var;

                if (value == "true") {
                    var = Var(true);
                } else if (value == "false") {
                    var = Var(false);
                } else if (value[0] == '\"') {
                    var = Var(value.substr(1, value.find_last_of('\"') - 1));
                } else {
                    var = Var(value);
                }

                IniSection& section = m_sections[section_name];
                if (section.name.empty()) {
                    section.name = section_name;
                }

                section.values.emplace(key, var);
            }
        }

        return WG_OK;
    }

}// namespace wmoge
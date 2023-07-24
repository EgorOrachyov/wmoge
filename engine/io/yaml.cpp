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

#include "yaml.hpp"

#include "core/engine.hpp"
#include "platform/file_system.hpp"

#include <cassert>
#include <sstream>
#include <string>

namespace wmoge {

    YamlTree yaml_parse(const std::vector<std::uint8_t>& data) {
        auto str_view = ryml::csubstr(reinterpret_cast<const char*>(data.data()), data.size());
        return ryml::parse_in_arena(str_view);
    }

    YamlTree yaml_parse_file(const std::string& file_path) {
        std::vector<std::uint8_t> file;

        if (!Engine::instance()->file_system()->read_file(file_path, file)) {
            WG_LOG_ERROR("failed to read content of file " << file_path);
            return {};
        }

        return yaml_parse(file);
    }

    bool yaml_read(const YamlConstNodeRef& node, bool& value) {
        node >> value;
        return true;
    }
    bool yaml_read(const YamlConstNodeRef& node, int& value) {
        node >> value;
        return true;
    }
    bool yaml_read(const YamlConstNodeRef& node, float& value) {
        node >> value;
        return true;
    }
    bool yaml_read(const YamlConstNodeRef& node, StringId& value) {
        std::string string;
        node >> string;
        value = SID(string);
        return true;
    }
    bool yaml_read(const YamlConstNodeRef& node, std::string& value) {
        node >> value;
        return true;
    }

    bool yaml_write(YamlNodeRef node, const bool& value) {
        node << value;
        return true;
    }
    bool yaml_write(YamlNodeRef node, const int& value) {
        node << value;
        return true;
    }
    bool yaml_write(YamlNodeRef node, const float& value) {
        node << value;
        return true;
    }
    bool yaml_write(YamlNodeRef node, const StringId& value) {
        node << value.str();
        return true;
    }
    bool yaml_write(YamlNodeRef node, const std::string& value) {
        node << value;
        return true;
    }

}// namespace wmoge
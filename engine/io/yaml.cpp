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

#include <cassert>
#include <sstream>
#include <string>

namespace wmoge {

    YamlTree Yaml::parse(const std::vector<std::uint8_t>& data) {
        auto str_view = ryml::csubstr(reinterpret_cast<const char*>(data.data()), data.size());
        return ryml::parse_in_arena(str_view);
    }
    bool Yaml::read(const YamlConstNodeRef& node, std::string& str) {
        node >> str;
        return true;
    }
    bool Yaml::read(const YamlConstNodeRef& node, StringId& sid) {
        std::string string;
        node >> string;
        sid = SID(string);
        return true;
    }
    bool Yaml::read(const YamlConstNodeRef& node, Vec2f& v) {
        std::string string;
        node >> string;
        std::stringstream stream(string);
        stream >> v[0] >> v[1];
        return true;
    }
    bool Yaml::read(const YamlConstNodeRef& node, Vec3f& v) {
        std::string string;
        node >> string;
        std::stringstream stream(string);
        stream >> v[0] >> v[1] >> v[2];
        return true;
    }
    bool Yaml::read(const YamlConstNodeRef& node, Vec4f& v) {
        std::string string;
        node >> string;
        std::stringstream stream(string);
        stream >> v[0] >> v[1] >> v[2] >> v[3];
        return true;
    }
    std::string Yaml::read_str(const YamlConstNodeRef& node) {
        std::string value;
        Yaml::read(node, value);
        return value;
    }
    StringId Yaml::read_sid(const YamlConstNodeRef& node) {
        StringId value;
        Yaml::read(node, value);
        return value;
    }
    bool Yaml::read_bool(const wmoge::YamlConstNodeRef& node) {
        bool value = false;
        node >> value;
        return value;
    }
    int Yaml::read_int(const YamlConstNodeRef& node) {
        int value = 0;
        node >> value;
        return value;
    }
    float Yaml::read_float(const YamlConstNodeRef& node) {
        float value = 0;
        node >> value;
        return value;
    }
    Vec2f Yaml::read_vec2f(const YamlConstNodeRef& node) {
        Vec2f value;
        Yaml::read(node, value);
        return value;
    }
    Vec3f Yaml::read_vec3f(const YamlConstNodeRef& node) {
        Vec3f value;
        Yaml::read(node, value);
        return value;
    }
    Vec4f Yaml::read_vec4f(const YamlConstNodeRef& node) {
        Vec4f value;
        Yaml::read(node, value);
        return value;
    }

}// namespace wmoge
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

#ifndef WMOGE_YAML_HPP
#define WMOGE_YAML_HPP

#include "core/string_id.hpp"
#include "math/vec.hpp"

#include <magic_enum.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <cinttypes>
#include <vector>

namespace wmoge {

    /**
     * @class YamlTree
     * @brief Represents parsed tree of yaml document
     */
    using YamlTree = ryml::Tree;

    /**
     * @class YamlNodeRef
     * @brief Reference to a node in yaml tree
     */
    using YamlNodeRef = ryml::NodeRef;

    /**
     * @class YamlConstNodeRef
     * @brief Constant reference to a node in yaml tree
     */
    using YamlConstNodeRef = ryml::ConstNodeRef;

    /**
     * @class Yaml
     * @brief Auxiliary class for yaml reading/writing
     */
    class Yaml {
    public:
        static YamlTree parse(const std::vector<std::uint8_t>& data);
        static bool     read(const YamlConstNodeRef& node, std::string& str);
        static bool     read(const YamlConstNodeRef& node, StringId& sid);
        static bool     read(const YamlConstNodeRef& node, Vec2f& v);
        static bool     read(const YamlConstNodeRef& node, Vec3f& v);
        static bool     read(const YamlConstNodeRef& node, Vec4f& v);

        static std::string read_str(const YamlConstNodeRef& node);
        static StringId    read_sid(const YamlConstNodeRef& node);
        static bool        read_bool(const YamlConstNodeRef& node);
        static int         read_int(const YamlConstNodeRef& node);
        static float       read_float(const YamlConstNodeRef& node);
        static Vec2f       read_vec2f(const YamlConstNodeRef& node);
        static Vec3f       read_vec3f(const YamlConstNodeRef& node);
        static Vec4f       read_vec4f(const YamlConstNodeRef& node);

        template<typename T>
        static bool read_enum(const YamlConstNodeRef& node, T& value) {
            std::string s;
            node >> s;
            auto parsed = magic_enum::enum_cast<T>(s);
            if (parsed.has_value()) {
                value = parsed.value();
            }
            return false;
        }
    };

}// namespace wmoge

#endif//WMOGE_YAML_HPP

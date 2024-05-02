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

#include "core/log.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "io/context.hpp"

#include <magic_enum.hpp>
#include <robin_hood.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <array>
#include <bitset>
#include <cassert>
#include <cinttypes>
#include <optional>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @brief Represents parsed tree of yaml document
     */
    using YamlTree = ryml::Tree;

    /**
     * @brief Reference to a node in yaml tree
     */
    using YamlNodeRef = ryml::NodeRef;

    /**
     * @brief Constant reference to a node in yaml tree
     */
    using YamlConstNodeRef = ryml::ConstNodeRef;

    /**
     * @brief Parse binary data into a tree
     *
     * @param data Data to parse, will not be modified
     *
     * @return Tree containing the data
     */
    YamlTree yaml_parse(const std::vector<std::uint8_t>& data);

    /**
     * @brief Opens and parses file
     *
     * @param file_path Path to the file
     *
     * @return Parse tree or empty tree
     */
    YamlTree yaml_parse_file(const std::string& file_path);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, bool& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const bool& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, int& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const int& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, unsigned int& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const unsigned int& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, float& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const float& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, Strid& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const Strid& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::string& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::string& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::int16_t& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::int16_t& value);

    Status yaml_read(IoContext& context, YamlConstNodeRef node, Status& value);
    Status yaml_write(IoContext& context, YamlNodeRef node, const Status& value);

    template<typename T>
    Status yaml_read_tree(YamlTree& tree, T& value) {
        IoContext context;
        return yaml_read(context, tree.crootref(), value);
    }

    template<typename T>
    Status yaml_read_file(const std::string& file_path, T& value) {
        YamlTree tree = yaml_parse_file(file_path);
        if (tree.empty()) {
            return StatusCode::FailedParse;
        }
        return yaml_read_tree(tree, value);
    }

#define WG_YAML_READ(context, node, what)                            \
    do {                                                             \
        if (!yaml_read(context, node, what)) {                       \
            WG_LOG_ERROR("failed to read yaml \"" << #what << "\""); \
            return StatusCode::FailedRead;                           \
        }                                                            \
    } while (false)

#define WG_YAML_READ_AS(context, node, node_name, what)                                 \
    do {                                                                                \
        if (!node.has_child(node_name) || !yaml_read(context, node[node_name], what)) { \
            WG_LOG_ERROR("failed to read yaml \"" << #what << "\"");                    \
            return StatusCode::FailedRead;                                              \
        }                                                                               \
    } while (false)

#define WG_YAML_READ_AS_OPT(context, node, node_name, what)              \
    do {                                                                 \
        if (node.has_child(node_name)) {                                 \
            if (!yaml_read(context, node[node_name], what)) {            \
                WG_LOG_ERROR("failed to read yaml \"" << #what << "\""); \
                return StatusCode::FailedRead;                           \
            }                                                            \
        }                                                                \
    } while (false)

#define WG_YAML_READ_SUPER(context, node, super, what)    \
    do {                                                  \
        WG_YAML_READ(context, node, *((super*) (&what))); \
    } while (false)

#define WG_YAML_WRITE(context, node, what)                            \
    do {                                                              \
        if (!yaml_write(context, node, what)) {                       \
            WG_LOG_ERROR("failed to write yaml \"" << #what << "\""); \
            return StatusCode::FailedWrite;                           \
        }                                                             \
    } while (false)

#define WG_YAML_WRITE_AS(context, node, node_name, what)              \
    do {                                                              \
        auto child = node.append_child();                             \
        child << ryml::key(node_name);                                \
        if (!yaml_write(context, child, what)) {                      \
            WG_LOG_ERROR("failed to write yaml \"" << #what << "\""); \
            return StatusCode::FailedWrite;                           \
        }                                                             \
    } while (false)

#define WG_YAML_WRITE_AS_OPT(node, node_name, condition, what)            \
    do {                                                                  \
        if (condition) {                                                  \
            auto child = node.append_child();                             \
            child << ryml::key(node_name);                                \
            if (!yaml_write(context, child, what)) {                      \
                WG_LOG_ERROR("failed to write yaml \"" << #what << "\""); \
                return StatusCode::FailedWrite;                           \
            }                                                             \
        }                                                                 \
    } while (false)

#define WG_YAML_WRITE_SUPER(node, super, what)          \
    do {                                                \
        WG_YAML_WRITE(node, *((const super*) (&what))); \
    } while (false)

#define WG_YAML_MAP(node) node |= ryml::MAP
#define WG_YAML_SEQ(node) node |= ryml::SEQ

    template<typename K, typename V>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::pair<K, V>& pair) {
        WG_YAML_READ_AS(context, node, "key", pair.first);
        WG_YAML_READ_AS(context, node, "value", pair.second);
        return WG_OK;
    }
    template<typename K, typename V>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::pair<K, V>& pair) {
        WG_YAML_MAP(node);

        YamlNodeRef key = node.append_child();
        WG_YAML_WRITE_AS(context, key, "key", pair.first);

        YamlNodeRef value = node.append_child();
        WG_YAML_WRITE_AS(context, value, "value", pair.second);

        return WG_OK;
    }

    template<typename K, typename V>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, robin_hood::pair<K, V>& pair) {
        WG_YAML_READ_AS(context, node, "key", pair.first);
        WG_YAML_READ_AS(context, node, "value", pair.second);
        return WG_OK;
    }
    template<typename K, typename V>
    Status yaml_write(IoContext& context, YamlNodeRef node, const robin_hood::pair<K, V>& pair) {
        WG_YAML_MAP(node);

        YamlNodeRef key = node.append_child();
        WG_YAML_WRITE_AS(context, key, "key", pair.first);

        YamlNodeRef value = node.append_child();
        WG_YAML_WRITE_AS(context, value, "value", pair.second);

        return WG_OK;
    }

    template<typename T, std::size_t S>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::array<T, S>& array) {
        std::size_t element_id = 0;
        assert(node.num_children() <= S);
        for (auto child = node.first_child(); child.valid() && element_id < S; child = child.next_sibling()) {
            WG_YAML_READ(context, child, array[element_id++]);
        }
        return WG_OK;
    }
    template<typename T, std::size_t S>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::array<T, S>& array) {
        WG_YAML_SEQ(node);
        for (std::size_t i = 0; i < S; i++) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(context, child, array[i]);
        }
        return WG_OK;
    }

    template<typename T>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::vector<T>& vector) {
        assert(vector.empty());
        vector.resize(node.num_children());
        std::size_t element_id = 0;
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            WG_YAML_READ(context, child, vector[element_id]);
            element_id += 1;
        }
        return WG_OK;
    }
    template<typename T>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::vector<T>& vector) {
        WG_YAML_SEQ(node);
        for (const T& value : vector) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(context, child, value);
        }
        return WG_OK;
    }

    template<typename T>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::unordered_set<T>& set) {
        assert(set.empty());
        set.reserve(node.num_children());
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            T entry;
            WG_YAML_READ(context, child, entry);
            set.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename T>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::unordered_set<T>& set) {
        WG_YAML_SEQ(node);
        for (const T& value : set) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(context, child, value);
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::unordered_map<K, V>& map) {
        assert(map.empty());
        map.reserve(node.num_children());
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            std::pair<K, V> entry;
            WG_YAML_READ(context, child, entry);
            map.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename K, typename V>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::unordered_map<K, V>& map) {
        WG_YAML_SEQ(node);
        for (const auto& entry : map) {
            YamlNodeRef entry_child = node.append_child();
            WG_YAML_WRITE(context, entry_child, entry);
        }
        return WG_OK;
    }

    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, T& enum_value) {
        std::string s;
        WG_YAML_READ(context, node, s);
        auto parsed = magic_enum::enum_cast<T>(s);
        if (!parsed.has_value()) {
            return StatusCode::FailedRead;
        }
        enum_value = parsed.value();
        return WG_OK;
    }
    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    Status yaml_write(IoContext& context, YamlNodeRef node, const T& enum_value) {
        node << std::string(magic_enum::enum_name(enum_value));
        return WG_OK;
    }

    template<typename T>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::optional<T>& wrapper) {
        if (!node.empty()) {
            wrapper.emplace();
            WG_YAML_READ(context, node, wrapper.value());
        }
        return WG_OK;
    }
    template<typename T>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::optional<T>& wrapper) {
        if (wrapper.has_value()) {
            WG_YAML_WRITE(context, node, wrapper.value());
        }
        return WG_OK;
    }

    template<std::size_t N>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::bitset<N>& bitset) {
        std::array<bool, N> values;
        WG_YAML_READ(context, node, values);
        for (std::size_t i = 0; i < N; i++) {
            if (values[i]) {
                bitset.set(i);
            }
        }
        return WG_OK;
    }
    template<std::size_t N>
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::bitset<N>& bitset) {
        std::array<bool, N> values;
        values.fill(false);
        for (std::size_t i = 0; i < N; i++) {
            if (bitset.test(i)) {
                values[i] = true;
            }
        }
        WG_YAML_WRITE(context, node, values);
        return WG_OK;
    }

}// namespace wmoge
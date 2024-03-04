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

    Status yaml_read(const YamlConstNodeRef& node, bool& value);
    Status yaml_read(const YamlConstNodeRef& node, int& value);
    Status yaml_read(const YamlConstNodeRef& node, float& value);
    Status yaml_read(const YamlConstNodeRef& node, Strid& value);
    Status yaml_read(const YamlConstNodeRef& node, std::string& value);
    Status yaml_read(const YamlConstNodeRef& node, std::int16_t& value);

    Status yaml_write(YamlNodeRef node, const bool& value);
    Status yaml_write(YamlNodeRef node, const int& value);
    Status yaml_write(YamlNodeRef node, const float& value);
    Status yaml_write(YamlNodeRef node, const Strid& value);
    Status yaml_write(YamlNodeRef node, const std::string& value);
    Status yaml_write(YamlNodeRef node, const std::int16_t& value);

#define WG_YAML_READ(node, what)                                     \
    do {                                                             \
        if (!yaml_read(node, what)) {                                \
            WG_LOG_ERROR("failed to read yaml \"" << #what << "\""); \
            return StatusCode::FailedRead;                           \
        }                                                            \
    } while (false)

#define WG_YAML_READ_AS(node, node_name, what)                                 \
    do {                                                                       \
        if (!node.has_child(node_name) || !yaml_read(node[node_name], what)) { \
            WG_LOG_ERROR("failed to read yaml \"" << #what << "\"");           \
            return StatusCode::FailedRead;                                     \
        }                                                                      \
    } while (false)

#define WG_YAML_READ_AS_OPT(node, node_name, what)                       \
    do {                                                                 \
        if (node.has_child(node_name)) {                                 \
            if (!yaml_read(node[node_name], what)) {                     \
                WG_LOG_ERROR("failed to read yaml \"" << #what << "\""); \
                return StatusCode::FailedRead;                           \
            }                                                            \
        }                                                                \
    } while (false)

#define WG_YAML_READ_SUPER(node, super, what)    \
    do {                                         \
        WG_YAML_READ(node, *((super*) (&what))); \
    } while (false)

#define WG_YAML_WRITE(node, what)                                     \
    do {                                                              \
        if (!yaml_write(node, what)) {                                \
            WG_LOG_ERROR("failed to write yaml \"" << #what << "\""); \
            return StatusCode::FailedWrite;                           \
        }                                                             \
    } while (false)

#define WG_YAML_WRITE_AS(node, node_name, what)                       \
    do {                                                              \
        auto child = node.append_child();                             \
        child << ryml::key(node_name);                                \
        if (!yaml_write(child, what)) {                               \
            WG_LOG_ERROR("failed to write yaml \"" << #what << "\""); \
            return StatusCode::FailedWrite;                           \
        }                                                             \
    } while (false)

#define WG_YAML_WRITE_AS_OPT(node, node_name, condition, what)            \
    do {                                                                  \
        if (condition) {                                                  \
            auto child = node.append_child();                             \
            child << ryml::key(node_name);                                \
            if (!yaml_write(child, what)) {                               \
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
    Status yaml_read(const YamlConstNodeRef& node, std::pair<K, V>& pair) {
        WG_YAML_READ_AS(node, "key", pair.first);
        WG_YAML_READ_AS(node, "value", pair.second);
        return StatusCode::Ok;
    }
    template<typename K, typename V>
    Status yaml_write(YamlNodeRef node, const std::pair<K, V>& pair) {
        WG_YAML_MAP(node);

        YamlNodeRef key = node.append_child();
        WG_YAML_WRITE_AS(key, "key", pair.first);

        YamlNodeRef value = node.append_child();
        WG_YAML_WRITE_AS(value, "value", pair.second);

        return StatusCode::Ok;
    }

    template<typename K, typename V>
    Status yaml_read(const YamlConstNodeRef& node, robin_hood::pair<K, V>& pair) {
        WG_YAML_READ_AS(node, "key", pair.first);
        WG_YAML_READ_AS(node, "value", pair.second);
        return StatusCode::Ok;
    }
    template<typename K, typename V>
    Status yaml_write(YamlNodeRef node, const robin_hood::pair<K, V>& pair) {
        WG_YAML_MAP(node);

        YamlNodeRef key = node.append_child();
        WG_YAML_WRITE_AS(key, "key", pair.first);

        YamlNodeRef value = node.append_child();
        WG_YAML_WRITE_AS(value, "value", pair.second);

        return StatusCode::Ok;
    }

    template<typename T, std::size_t S>
    Status yaml_read(const YamlConstNodeRef& node, std::array<T, S>& array) {
        std::size_t element_id = 0;
        assert(node.num_children() <= S);
        for (auto child = node.first_child(); child.valid() && element_id < S; child = child.next_sibling()) {
            WG_YAML_READ(child, array[element_id++]);
        }
        return StatusCode::Ok;
    }
    template<typename T, std::size_t S>
    Status yaml_write(YamlNodeRef node, const std::array<T, S>& array) {
        WG_YAML_SEQ(node);
        for (std::size_t i = 0; i < S; i++) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(child, array[i]);
        }
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, std::vector<T>& vector) {
        assert(vector.empty());
        vector.resize(node.num_children());
        std::size_t element_id = 0;
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            WG_YAML_READ(child, vector[element_id]);
            element_id += 1;
        }
        return StatusCode::Ok;
    }
    template<typename T>
    Status yaml_write(YamlNodeRef node, const std::vector<T>& vector) {
        WG_YAML_SEQ(node);
        for (const T& value : vector) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(child, value);
        }
        return StatusCode::Ok;
    }

    template<typename K, typename V>
    Status yaml_read(const YamlConstNodeRef& node, std::unordered_map<K, V>& map) {
        assert(map.empty());
        map.reserve(node.num_children());
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            std::pair<K, V> entry;
            WG_YAML_READ(child, entry);
            map.insert(std::move(entry));
        }
        return StatusCode::Ok;
    }
    template<typename K, typename V>
    Status yaml_write(YamlNodeRef node, const std::unordered_map<K, V>& map) {
        WG_YAML_SEQ(node);
        for (const auto& entry : map) {
            YamlNodeRef entry_child = node.append_child();
            WG_YAML_WRITE(entry_child, entry);
        }
        return StatusCode::Ok;
    }

    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    Status yaml_read(const YamlConstNodeRef& node, T& enum_value) {
        std::string s;
        WG_YAML_READ(node, s);
        auto parsed = magic_enum::enum_cast<T>(s);
        if (!parsed.has_value()) {
            return StatusCode::FailedRead;
        }
        enum_value = parsed.value();
        return StatusCode::Ok;
    }
    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    Status yaml_write(YamlNodeRef node, const T& enum_value) {
        node << std::string(magic_enum::enum_name(enum_value));
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, std::optional<T>& wrapper) {
        wrapper.emplace();
        WG_YAML_READ(node, wrapper.value());
        return StatusCode::Ok;
    }
    template<typename T>
    Status yaml_write(YamlNodeRef node, const std::optional<T>& wrapper) {
        assert(wrapper.has_value());
        WG_YAML_WRITE(node, wrapper.value());
        return StatusCode::Ok;
    }

    template<std::size_t N>
    Status yaml_read(const YamlConstNodeRef& node, std::bitset<N>& bitset) {
        std::array<bool, N> values;
        WG_YAML_READ(node, values);
        for (std::size_t i = 0; i < N; i++) {
            if (values[i]) {
                bitset.set(i);
            }
        }
        return StatusCode::Ok;
    }
    template<std::size_t N>
    Status yaml_write(YamlNodeRef node, const std::bitset<N>& bitset) {
        std::array<bool, N> values;
        values.fill(false);
        for (std::size_t i = 0; i < N; i++) {
            if (bitset.test(i)) {
                values[i] = true;
            }
        }
        WG_YAML_WRITE(node, values);
        return StatusCode::Ok;
    }

}// namespace wmoge
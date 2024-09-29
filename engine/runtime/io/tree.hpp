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
#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "io/context.hpp"

#include <magic_enum.hpp>
#include <robin_hood.hpp>

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

    /** @brief Io tree flags to control tree serialization */
    enum class IoTreeFlag {
        FormatText = 0,
        FormatBinary,
        FormatDirect,
        FormatVariant,
        UserFriendly
    };

    /** @brief Io tree flags mask */
    using IoTreeFlags = Mask<IoTreeFlag>;

    /**
     * @class IoTree
     * @brief Structured property serialization and de-serialization tree
     */
    class IoTree {
    public:
        virtual ~IoTree() = default;

        virtual bool        node_is_empty()                               = 0;
        virtual bool        node_has_child(const std::string_view& name)  = 0;
        virtual Status      node_find_child(const std::string_view& name) = 0;
        virtual Status      node_append_child()                           = 0;
        virtual void        node_find_first_child()                       = 0;
        virtual bool        node_is_valid()                               = 0;
        virtual void        node_next_sibling()                           = 0;
        virtual void        node_pop()                                    = 0;
        virtual std::size_t node_num_children()                           = 0;

        virtual Status node_write_key(const std::string_view& key) = 0;

        virtual Status node_write_value(const bool& value)         = 0;
        virtual Status node_write_value(const int& value)          = 0;
        virtual Status node_write_value(const unsigned int& value) = 0;
        virtual Status node_write_value(const float& value)        = 0;
        virtual Status node_write_value(const std::string& value)  = 0;
        virtual Status node_write_value(const Strid& value)        = 0;
        virtual Status node_write_value(const std::int16_t& value) = 0;
        virtual Status node_write_value(const std::size_t& value)  = 0;

        virtual Status node_read_value(bool& value)         = 0;
        virtual Status node_read_value(int& value)          = 0;
        virtual Status node_read_value(unsigned int& value) = 0;
        virtual Status node_read_value(float& value)        = 0;
        virtual Status node_read_value(std::string& value)  = 0;
        virtual Status node_read_value(Strid& value)        = 0;
        virtual Status node_read_value(std::int16_t& value) = 0;
        virtual Status node_read_value(std::size_t& value)  = 0;

        virtual void node_as_map()                    = 0;
        virtual void node_as_list(std::size_t length) = 0;

        [[nodiscard]] bool               can_read() const { return m_can_read; }
        [[nodiscard]] bool               can_write() const { return m_can_write; }
        [[nodiscard]] const Strid&       get_name() const { return m_name; }
        [[nodiscard]] const IoTreeFlags& get_flags() const { return m_flags; }

    protected:
        Strid       m_name;
        IoTreeFlags m_flags;
        bool        m_can_read  = false;
        bool        m_can_write = false;
    };

    Status tree_read(IoContext& context, IoTree& tree, bool& value);
    Status tree_write(IoContext& context, IoTree& tree, const bool& value);

    Status tree_read(IoContext& context, IoTree& tree, int& value);
    Status tree_write(IoContext& context, IoTree& tree, const int& value);

    Status tree_read(IoContext& context, IoTree& tree, unsigned int& value);
    Status tree_write(IoContext& context, IoTree& tree, const unsigned int& value);

    Status tree_read(IoContext& context, IoTree& tree, float& value);
    Status tree_write(IoContext& context, IoTree& tree, const float& value);

    Status tree_read(IoContext& context, IoTree& tree, Strid& value);
    Status tree_write(IoContext& context, IoTree& tree, const Strid& value);

    Status tree_read(IoContext& context, IoTree& tree, std::string& value);
    Status tree_write(IoContext& context, IoTree& tree, const std::string& value);

    Status tree_read(IoContext& context, IoTree& tree, std::int16_t& value);
    Status tree_write(IoContext& context, IoTree& tree, const std::int16_t& value);

    Status tree_read(IoContext& context, IoTree& tree, std::size_t& value);
    Status tree_write(IoContext& context, IoTree& tree, const std::size_t& value);

    Status tree_read(IoContext& context, IoTree& tree, Status& value);
    Status tree_write(IoContext& context, IoTree& tree, const Status& value);

#define WG_TREE_READ(context, tree, what)                            \
    do {                                                             \
        if (!tree_read(context, tree, what)) {                       \
            WG_LOG_ERROR("failed to read tree \"" << #what << "\""); \
            return StatusCode::FailedRead;                           \
        }                                                            \
    } while (false)

#define WG_TREE_READ_AS(context, tree, node_name, what)                     \
    do {                                                                    \
        if (!tree.node_find_child(node_name)) {                             \
            WG_LOG_ERROR("failed to fetch child tree \"" << #what << "\""); \
            return StatusCode::FailedRead;                                  \
        }                                                                   \
        if (!tree_read(context, tree, what)) {                              \
            WG_LOG_ERROR("failed to read tree \"" << #what << "\"");        \
            return StatusCode::FailedRead;                                  \
        }                                                                   \
        tree.node_pop();                                                    \
    } while (false)

#define WG_TREE_READ_AS_OPT(context, tree, node_name, what)                     \
    do {                                                                        \
        if (tree.node_has_child(node_name)) {                                   \
            if (!tree.node_find_child(node_name)) {                             \
                WG_LOG_ERROR("failed to fetch child tree \"" << #what << "\""); \
                return StatusCode::FailedRead;                                  \
            }                                                                   \
            if (!tree_read(context, tree, what)) {                              \
                WG_LOG_ERROR("failed to read tree \"" << #what << "\"");        \
                return StatusCode::FailedRead;                                  \
            }                                                                   \
            tree.node_pop();                                                    \
        }                                                                       \
    } while (false)

#define WG_TREE_READ_SUPER(context, tree, super, what)    \
    do {                                                  \
        WG_TREE_READ(context, tree, *((super*) (&what))); \
    } while (false)

#define WG_TREE_WRITE(context, tree, what)                            \
    do {                                                              \
        if (!tree_write(context, tree, what)) {                       \
            WG_LOG_ERROR("failed to write tree \"" << #what << "\""); \
            return StatusCode::FailedWrite;                           \
        }                                                             \
    } while (false)

#define WG_TREE_WRITE_AS(context, tree, node_name, what)                     \
    do {                                                                     \
        if (!tree.node_append_child()) {                                     \
            WG_LOG_ERROR("failed to append child tree \"" << #what << "\""); \
            return StatusCode::FailedWrite;                                  \
        }                                                                    \
        tree.node_write_key(node_name);                                      \
        if (!tree_write(context, tree, what)) {                              \
            WG_LOG_ERROR("failed to write tree \"" << #what << "\"");        \
            return StatusCode::FailedWrite;                                  \
        }                                                                    \
        tree.node_pop();                                                     \
    } while (false)

#define WG_TREE_WRITE_SUPER(tree, super, what)          \
    do {                                                \
        WG_TREE_WRITE(tree, *((const super*) (&what))); \
    } while (false)

#define WG_TREE_MAP(tree)         tree.node_as_map()
#define WG_TREE_SEQ(tree, length) tree.node_as_list(length)

    template<typename K, typename V>
    Status tree_read(IoContext& context, IoTree& tree, std::pair<K, V>& pair) {
        WG_TREE_READ_AS(context, tree, "key", pair.first);
        WG_TREE_READ_AS(context, tree, "value", pair.second);
        return WG_OK;
    }
    template<typename K, typename V>
    Status tree_write(IoContext& context, IoTree& tree, const std::pair<K, V>& pair) {
        WG_TREE_MAP(tree);
        WG_TREE_WRITE_AS(context, tree, "key", pair.first);
        WG_TREE_WRITE_AS(context, tree, "value", pair.second);
        return WG_OK;
    }

    template<typename K, typename V>
    Status tree_read(IoContext& context, IoTree& tree, robin_hood::pair<K, V>& pair) {
        WG_TREE_READ_AS(context, tree, "key", pair.first);
        WG_TREE_READ_AS(context, tree, "value", pair.second);
        return WG_OK;
    }
    template<typename K, typename V>
    Status tree_write(IoContext& context, IoTree& tree, const robin_hood::pair<K, V>& pair) {
        WG_TREE_MAP(tree);
        WG_TREE_WRITE_AS(context, tree, "key", pair.first);
        WG_TREE_WRITE_AS(context, tree, "value", pair.second);
        return WG_OK;
    }

    template<typename T, std::size_t S>
    Status tree_read(IoContext& context, IoTree& tree, std::array<T, S>& array) {
        std::size_t element_id = 0;
        assert(tree.node_num_children() <= S);
        tree.node_find_first_child();
        for (; tree.node_is_valid() && element_id < S; tree.node_next_sibling()) {
            WG_TREE_READ(context, tree, array[element_id]);
            element_id++;
        }
        return WG_OK;
    }
    template<typename T, std::size_t S>
    Status tree_write(IoContext& context, IoTree& tree, const std::array<T, S>& array) {
        WG_TREE_SEQ(tree, S);
        for (std::size_t i = 0; i < S; i++) {
            WG_CHECKED(tree.node_append_child());
            WG_TREE_WRITE(context, tree, array[i]);
            tree.node_pop();
        }
        return WG_OK;
    }

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, std::vector<T>& vector) {
        assert(vector.empty());
        vector.resize(tree.node_num_children());
        std::size_t element_id = 0;
        tree.node_find_first_child();
        for (; tree.node_is_valid(); tree.node_next_sibling()) {
            WG_TREE_READ(context, tree, vector[element_id]);
            element_id++;
        }
        return WG_OK;
    }
    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const std::vector<T>& vector) {
        WG_TREE_SEQ(tree, vector.size());
        for (const T& value : vector) {
            WG_CHECKED(tree.node_append_child());
            WG_TREE_WRITE(context, tree, value);
            tree.node_pop();
        }
        return WG_OK;
    }

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, std::unordered_set<T>& set) {
        assert(set.empty());
        set.reserve(tree.node_num_children());
        tree.node_find_first_child();
        for (; tree.node_is_valid(); tree.node_next_sibling()) {
            T entry;
            WG_TREE_READ(context, tree, entry);
            set.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const std::unordered_set<T>& set) {
        WG_TREE_SEQ(tree, set.size());
        for (const T& entry : set) {
            WG_CHECKED(tree.node_append_child());
            WG_TREE_WRITE(context, tree, entry);
            tree.node_pop();
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status tree_read(IoContext& context, IoTree& tree, std::unordered_map<K, V>& map) {
        assert(map.empty());
        map.reserve(tree.node_num_children());
        tree.node_find_first_child();
        for (; tree.node_is_valid(); tree.node_next_sibling()) {
            std::pair<K, V> entry;
            WG_TREE_READ(context, tree, entry);
            map.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename K, typename V>
    Status tree_write(IoContext& context, IoTree& tree, const std::unordered_map<K, V>& map) {
        WG_TREE_SEQ(tree, map.size());
        for (const auto& entry : map) {
            WG_CHECKED(tree.node_append_child());
            WG_TREE_WRITE(context, tree, entry);
            tree.node_pop();
        }
        return WG_OK;
    }

    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    Status tree_read(IoContext& context, IoTree& tree, T& enum_value) {
        std::string s;
        WG_TREE_READ(context, tree, s);
        auto parsed = magic_enum::enum_cast<T>(s);
        if (!parsed.has_value()) {
            return StatusCode::FailedRead;
        }
        enum_value = parsed.value();
        return WG_OK;
    }
    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    Status tree_write(IoContext& context, IoTree& tree, const T& enum_value) {
        WG_TREE_WRITE(context, tree, std::string(magic_enum::enum_name(enum_value)));
        return WG_OK;
    }

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, std::optional<T>& wrapper) {
        if (!tree.node_is_empty()) {
            wrapper.emplace();
            WG_TREE_READ(context, tree, wrapper.value());
        }
        return WG_OK;
    }
    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const std::optional<T>& wrapper) {
        if (wrapper.has_value()) {
            WG_TREE_WRITE(context, tree, wrapper.value());
        }
        return WG_OK;
    }

    template<std::size_t N>
    Status tree_read(IoContext& context, IoTree& tree, std::bitset<N>& bitset) {
        std::array<bool, N> values;
        WG_TREE_READ(context, tree, values);
        for (std::size_t i = 0; i < N; i++) {
            if (values[i]) {
                bitset.set(i);
            }
        }
        return WG_OK;
    }
    template<std::size_t N>
    Status tree_write(IoContext& context, IoTree& tree, const std::bitset<N>& bitset) {
        std::array<bool, N> values;
        values.fill(false);
        for (std::size_t i = 0; i < N; i++) {
            if (bitset.test(i)) {
                values[i] = true;
            }
        }
        WG_TREE_WRITE(context, tree, values);
        return WG_OK;
    }

    template<typename T, int size>
    Status tree_read(IoContext& context, IoTree& tree, Mask<T, size>& mask) {
        std::vector<T> flags;
        WG_TREE_READ(context, tree, flags);

        for (auto flag : flags) {
            mask.set(flag);
        }

        return WG_OK;
    }

    template<typename T, int size>
    Status tree_write(IoContext& context, IoTree& tree, const Mask<T, size>& mask) {
        std::vector<T> flags;

        mask.for_each([&](int, T flag) {
            flags.push_back(flag);
        });

        WG_TREE_WRITE(context, tree, flags);
        return WG_OK;
    }

}// namespace wmoge
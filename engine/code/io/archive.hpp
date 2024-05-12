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

#include <array>
#include <bitset>
#include <cinttypes>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @class Archive
     * @brief Serialization and de-serialization archive
     *
     * Archive abstracts the way where the data is stored and which form
     * is used to serialize the data. Archive allows to build a complex serialization
     * logic with nesting if the data, using recursive serialization pattern.
     */
    class Archive : public RefCnt {
    public:
        ~Archive() override = default;

        virtual Status nwrite(int num_bytes, const void* bytes) { return StatusCode::NotImplemented; };
        virtual Status nread(int num_bytes, void* bytes) { return StatusCode::NotImplemented; };

        [[nodiscard]] virtual bool        is_memory()   = 0;
        [[nodiscard]] virtual bool        is_physical() = 0;
        [[nodiscard]] virtual std::size_t get_size()    = 0;

        [[nodiscard]] bool         can_read() const { return m_can_read; }
        [[nodiscard]] bool         can_write() const { return m_can_write; }
        [[nodiscard]] const Strid& get_name() const { return m_name; }

    protected:
        Strid m_name;
        bool  m_can_read  = false;
        bool  m_can_write = false;
    };

    template<typename T>
    Status archive_read(IoContext& context, Archive& archive, T& value, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr) {
        assert(archive.can_read());
        return archive.nread(sizeof(T), &value);
    }
    template<typename T>
    Status archive_write(IoContext& context, Archive& archive, const T& value, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr) {
        assert(archive.can_write());
        return archive.nwrite(sizeof(T), &value);
    }

    template<typename T>
    Status archive_read(IoContext& context, Archive& archive, T& value, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
        assert(archive.can_read());
        return archive.nread(sizeof(T), &value);
    }
    template<typename T>
    Status archive_write(IoContext& context, Archive& archive, const T& value, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
        assert(archive.can_write());
        return archive.nwrite(sizeof(T), &value);
    }

    template<std::size_t N>
    Status archive_read(IoContext& context, Archive& archive, std::bitset<N>& bitset) {
        return archive.nread(sizeof(std::bitset<N>), &bitset);
    }
    template<std::size_t N>
    Status archive_write(IoContext& context, Archive& archive, const std::bitset<N>& bitset) {
        return archive.nwrite(sizeof(std::bitset<N>), &bitset);
    }

    Status archive_read(IoContext& context, Archive& archive, bool& value);
    Status archive_write(IoContext& context, Archive& archive, const bool& value);

    Status archive_read(IoContext& context, Archive& archive, Strid& value);
    Status archive_write(IoContext& context, Archive& archive, const Strid& value);

    Status archive_read(IoContext& context, Archive& archive, std::string& value);
    Status archive_write(IoContext& context, Archive& archive, const std::string& value);

    Status archive_read(IoContext& context, Archive& archive, Status& value);
    Status archive_write(IoContext& context, Archive& archive, const Status& value);

#define WG_ARCHIVE_READ(context, archive, what)      \
    do {                                             \
        if (!archive_read(context, archive, what)) { \
            return StatusCode::FailedRead;           \
        }                                            \
    } while (false)

#define WG_ARCHIVE_WRITE(context, archive, what)      \
    do {                                              \
        if (!archive_write(context, archive, what)) { \
            return StatusCode::FailedWrite;           \
        }                                             \
    } while (false)

#define WG_ARCHIVE_READ_SUPER(context, archive, super, what) \
    WG_ARCHIVE_READ(context, archive, *((super*) &what))

#define WG_ARCHIVE_WRITE_SUPER(context, archive, super, what) \
    WG_ARCHIVE_WRITE(context, archive, *((const super*) &what))

    template<typename T, typename K>
    Status archive_read(IoContext& context, Archive& archive, std::pair<K, T>& pair) {
        WG_ARCHIVE_READ(context, archive, pair.first);
        WG_ARCHIVE_READ(context, archive, pair.second);
        return WG_OK;
    }
    template<typename T, typename K>
    Status archive_write(IoContext& context, Archive& archive, const std::pair<K, T>& pair) {
        WG_ARCHIVE_WRITE(context, archive, pair.first);
        WG_ARCHIVE_WRITE(context, archive, pair.second);
        return WG_OK;
    }

    template<typename T, std::size_t S>
    Status archive_read(IoContext& context, Archive& archive, std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            WG_ARCHIVE_READ(context, archive, array[i]);
        }
        return WG_OK;
    }
    template<typename T, std::size_t S>
    Status archive_write(IoContext& context, Archive& archive, const std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            WG_ARCHIVE_WRITE(context, archive, array[i]);
        }
        return WG_OK;
    }

    template<typename T>
    Status archive_read(IoContext& context, Archive& archive, std::vector<T>& vector) {
        assert(vector.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, archive, size);
        vector.resize(size);
        for (std::size_t i = 0; i < size; i++) {
            WG_ARCHIVE_READ(context, archive, vector[i]);
        }
        return WG_OK;
    }
    template<typename T>
    Status archive_write(IoContext& context, Archive& archive, const std::vector<T>& vector) {
        WG_ARCHIVE_WRITE(context, archive, vector.size());
        for (const auto& entry : vector) {
            WG_ARCHIVE_WRITE(context, archive, entry);
        }
        return WG_OK;
    }

    template<typename T>
    Status archive_read(IoContext& context, Archive& archive, std::unordered_set<T>& set) {
        assert(set.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, archive, size);
        set.reserve(size);
        for (std::size_t i = 0; i < size; i++) {
            T entry;
            WG_ARCHIVE_READ(context, archive, entry);
            set.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename T>
    Status archive_write(IoContext& context, Archive& archive, const std::unordered_set<T>& set) {
        WG_ARCHIVE_WRITE(context, archive, set.size());
        for (const auto& entry : set) {
            WG_ARCHIVE_WRITE(context, archive, entry);
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status archive_read(IoContext& context, Archive& archive, std::unordered_map<K, V>& map) {
        assert(map.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, archive, size);
        for (std::size_t i = 0; i < size; i++) {
            std::pair<K, V> entry;
            WG_ARCHIVE_READ(context, archive, entry.first);
            WG_ARCHIVE_READ(context, archive, entry.second);
            map.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename K, typename V>
    Status archive_write(IoContext& context, Archive& archive, const std::unordered_map<K, V>& map) {
        WG_ARCHIVE_WRITE(context, archive, map.size());
        for (const auto& entry : map) {
            WG_ARCHIVE_WRITE(context, archive, entry.first);
            WG_ARCHIVE_WRITE(context, archive, entry.second);
        }
        return WG_OK;
    }

    template<class T>
    Status archive_read(IoContext& context, Archive& archive, T& enum_value, typename std::enable_if_t<std::is_enum_v<T>>* = nullptr) {
        int value;
        WG_ARCHIVE_READ(context, archive, value);
        enum_value = static_cast<T>(value);
        return WG_OK;
    }
    template<class T>
    Status archive_write(IoContext& context, Archive& archive, const T& enum_value, typename std::enable_if_t<std::is_enum_v<T>>* = nullptr) {
        int value = static_cast<int>(enum_value);
        WG_ARCHIVE_WRITE(context, archive, value);
        return WG_OK;
    }

    template<class T>
    Status archive_read(IoContext& context, Archive& archive, std::optional<T>& opt) {
        bool has_value = false;
        WG_ARCHIVE_READ(context, archive, has_value);
        if (has_value) {
            T v;
            WG_ARCHIVE_READ(context, archive, v);
            opt = std::move(v);
        }
        return WG_OK;
    }
    template<class T>
    Status archive_write(IoContext& context, Archive& archive, const std::optional<T>& opt) {
        const bool has_value = opt.has_value();
        WG_ARCHIVE_WRITE(context, archive, has_value);
        if (has_value) {
            WG_ARCHIVE_WRITE(context, archive, opt.value());
        }
        return WG_OK;
    }

}// namespace wmoge
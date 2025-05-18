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

#include <array>
#include <bitset>
#include <cinttypes>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @class IoStream
     * @brief Serialization and de-serialization stream
     *
     * IoStream abstracts the way where the data is stored and which form
     * is used to serialize the data. IoStream allows to build a complex serialization
     * logic with nesting if the data, using recursive serialization pattern.
     */
    class IoStream : public RefCnt {
    public:
        ~IoStream() override = default;

        virtual Status nwrite(std::size_t num_bytes, const void* bytes) { return StatusCode::NotImplemented; };
        virtual Status nread(std::size_t num_bytes, void* bytes) { return StatusCode::NotImplemented; };

        [[nodiscard]] bool         can_read() const { return m_can_read; }
        [[nodiscard]] bool         can_write() const { return m_can_write; }
        [[nodiscard]] const Strid& get_name() const { return m_name; }

    protected:
        Strid m_name;
        bool  m_can_read  = false;
        bool  m_can_write = false;
    };

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, T& value, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr) {
        assert(stream.can_read());
        return stream.nread(sizeof(T), &value);
    }
    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const T& value, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr) {
        assert(stream.can_write());
        return stream.nwrite(sizeof(T), &value);
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, T& value, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
        assert(stream.can_read());
        return stream.nread(sizeof(T), &value);
    }
    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const T& value, typename std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
        assert(stream.can_write());
        return stream.nwrite(sizeof(T), &value);
    }

    template<std::size_t N>
    Status stream_read(IoContext& context, IoStream& stream, std::bitset<N>& bitset) {
        return stream.nread(sizeof(std::bitset<N>), &bitset);
    }
    template<std::size_t N>
    Status stream_write(IoContext& context, IoStream& stream, const std::bitset<N>& bitset) {
        return stream.nwrite(sizeof(std::bitset<N>), &bitset);
    }

    Status stream_read(IoContext& context, IoStream& stream, bool& value);
    Status stream_write(IoContext& context, IoStream& stream, const bool& value);

    Status stream_read(IoContext& context, IoStream& stream, Strid& value);
    Status stream_write(IoContext& context, IoStream& stream, const Strid& value);

    Status stream_read(IoContext& context, IoStream& stream, std::string& value);
    Status stream_write(IoContext& context, IoStream& stream, const std::string& value);

    Status stream_read(IoContext& context, IoStream& stream, Status& value);
    Status stream_write(IoContext& context, IoStream& stream, const Status& value);

#define WG_ARCHIVE_READ(context, stream, what)     \
    do {                                           \
        if (!stream_read(context, stream, what)) { \
            return StatusCode::FailedRead;         \
        }                                          \
    } while (false)

#define WG_ARCHIVE_WRITE(context, stream, what)     \
    do {                                            \
        if (!stream_write(context, stream, what)) { \
            return StatusCode::FailedWrite;         \
        }                                           \
    } while (false)

#define WG_ARCHIVE_READ_SUPER(context, stream, super, what) \
    WG_ARCHIVE_READ(context, stream, *((super*) &what))

#define WG_ARCHIVE_WRITE_SUPER(context, stream, super, what) \
    WG_ARCHIVE_WRITE(context, stream, *((const super*) &what))

    template<typename T, typename K>
    Status stream_read(IoContext& context, IoStream& stream, std::pair<K, T>& pair) {
        WG_ARCHIVE_READ(context, stream, pair.first);
        WG_ARCHIVE_READ(context, stream, pair.second);
        return WG_OK;
    }
    template<typename T, typename K>
    Status stream_write(IoContext& context, IoStream& stream, const std::pair<K, T>& pair) {
        WG_ARCHIVE_WRITE(context, stream, pair.first);
        WG_ARCHIVE_WRITE(context, stream, pair.second);
        return WG_OK;
    }

    template<typename T, std::size_t S>
    Status stream_read(IoContext& context, IoStream& stream, std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            WG_ARCHIVE_READ(context, stream, array[i]);
        }
        return WG_OK;
    }
    template<typename T, std::size_t S>
    Status stream_write(IoContext& context, IoStream& stream, const std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            WG_ARCHIVE_WRITE(context, stream, array[i]);
        }
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, std::vector<T>& vector) {
        assert(vector.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, stream, size);
        vector.resize(size);
        for (std::size_t i = 0; i < size; i++) {
            WG_ARCHIVE_READ(context, stream, vector[i]);
        }
        return WG_OK;
    }
    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const std::vector<T>& vector) {
        WG_ARCHIVE_WRITE(context, stream, vector.size());
        for (const auto& entry : vector) {
            WG_ARCHIVE_WRITE(context, stream, entry);
        }
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, std::unordered_set<T>& set) {
        assert(set.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, stream, size);
        set.reserve(size);
        for (std::size_t i = 0; i < size; i++) {
            T entry;
            WG_ARCHIVE_READ(context, stream, entry);
            set.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const std::unordered_set<T>& set) {
        WG_ARCHIVE_WRITE(context, stream, set.size());
        for (const auto& entry : set) {
            WG_ARCHIVE_WRITE(context, stream, entry);
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status stream_read(IoContext& context, IoStream& stream, std::unordered_map<K, V>& map) {
        assert(map.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, stream, size);
        for (std::size_t i = 0; i < size; i++) {
            std::pair<K, V> entry;
            WG_ARCHIVE_READ(context, stream, entry.first);
            WG_ARCHIVE_READ(context, stream, entry.second);
            map.insert(std::move(entry));
        }
        return WG_OK;
    }
    template<typename K, typename V>
    Status stream_write(IoContext& context, IoStream& stream, const std::unordered_map<K, V>& map) {
        WG_ARCHIVE_WRITE(context, stream, map.size());
        for (const auto& entry : map) {
            WG_ARCHIVE_WRITE(context, stream, entry.first);
            WG_ARCHIVE_WRITE(context, stream, entry.second);
        }
        return WG_OK;
    }

    template<class T>
    Status stream_read(IoContext& context, IoStream& stream, T& enum_value, typename std::enable_if_t<std::is_enum_v<T>>* = nullptr) {
        int value;
        WG_ARCHIVE_READ(context, stream, value);
        enum_value = static_cast<T>(value);
        return WG_OK;
    }
    template<class T>
    Status stream_write(IoContext& context, IoStream& stream, const T& enum_value, typename std::enable_if_t<std::is_enum_v<T>>* = nullptr) {
        int value = static_cast<int>(enum_value);
        WG_ARCHIVE_WRITE(context, stream, value);
        return WG_OK;
    }

    template<class T>
    Status stream_read(IoContext& context, IoStream& stream, std::optional<T>& opt) {
        bool has_value = false;
        WG_ARCHIVE_READ(context, stream, has_value);
        if (has_value) {
            T v;
            WG_ARCHIVE_READ(context, stream, v);
            opt = std::move(v);
        }
        return WG_OK;
    }
    template<class T>
    Status stream_write(IoContext& context, IoStream& stream, const std::optional<T>& opt) {
        const bool has_value = opt.has_value();
        WG_ARCHIVE_WRITE(context, stream, has_value);
        if (has_value) {
            WG_ARCHIVE_WRITE(context, stream, opt.value());
        }
        return WG_OK;
    }

    template<typename T, int size>
    Status stream_read(IoContext& context, IoStream& stream, Mask<T, size>& mask) {
        return stream.nread(sizeof(Mask<T, size>), &mask);
    }

    template<typename T, int size>
    Status stream_write(IoContext& context, IoStream& stream, const Mask<T, size>& mask) {
        return stream.nwrite(sizeof(Mask<T, size>), &mask);
    }

}// namespace wmoge
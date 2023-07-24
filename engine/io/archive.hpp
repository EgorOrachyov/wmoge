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

#ifndef WMOGE_ARCHIVE_HPP
#define WMOGE_ARCHIVE_HPP

#include "core/log.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"

#include <array>
#include <cinttypes>
#include <unordered_map>
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

        virtual bool nwrite(int num_bytes, const void* bytes) { return false; };
        virtual bool nread(int num_bytes, void* bytes) { return false; };

        virtual bool write(bool value);
        virtual bool write(int value);
        virtual bool write(float value);
        virtual bool write(std::size_t value);
        virtual bool write(const StringId& value);
        virtual bool write(const std::string& value);

        virtual bool read(bool& value);
        virtual bool read(int& value);
        virtual bool read(float& value);
        virtual bool read(std::size_t& value);
        virtual bool read(StringId& value);
        virtual bool read(std::string& value);

        [[nodiscard]] virtual bool        is_memory()   = 0;
        [[nodiscard]] virtual bool        is_physical() = 0;
        [[nodiscard]] virtual std::size_t get_size()    = 0;

        [[nodiscard]] bool            can_read() const { return m_can_read; }
        [[nodiscard]] bool            can_write() const { return m_can_write; }
        [[nodiscard]] const StringId& get_name() const { return m_name; }

    protected:
        StringId m_name;
        bool     m_can_read  = false;
        bool     m_can_write = false;
    };

    bool archive_write(Archive& archive, const bool& value);
    bool archive_write(Archive& archive, const int& value);
    bool archive_write(Archive& archive, const float& value);
    bool archive_write(Archive& archive, const std::size_t& value);
    bool archive_write(Archive& archive, const StringId& value);
    bool archive_write(Archive& archive, const std::string& value);

    bool archive_read(Archive& archive, bool& value);
    bool archive_read(Archive& archive, int& value);
    bool archive_read(Archive& archive, float& value);
    bool archive_read(Archive& archive, std::size_t& value);
    bool archive_read(Archive& archive, StringId& value);
    bool archive_read(Archive& archive, std::string& value);

#define WG_ARCHIVE_READ(archive, what)      \
    do {                                    \
        if (!archive_read(archive, what)) { \
            return false;                   \
        }                                   \
    } while (false)

#define WG_ARCHIVE_WRITE(archive, what)      \
    do {                                     \
        if (!archive_write(archive, what)) { \
            return false;                    \
        }                                    \
    } while (false)

    template<typename T, std::size_t S>
    bool archive_write(Archive& archive, const std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            WG_ARCHIVE_WRITE(archive, array[i]);
        }
        return true;
    }
    template<typename T>
    bool archive_write(Archive& archive, const std::vector<T>& vector) {
        WG_ARCHIVE_WRITE(archive, vector.size());
        for (const auto& entry : vector) {
            WG_ARCHIVE_WRITE(archive, entry);
        }
        return true;
    }
    template<typename K, typename V>
    bool archive_write(Archive& archive, const std::unordered_map<K, V>& map) {
        WG_ARCHIVE_WRITE(archive, map.size());
        for (const auto& entry : map) {
            WG_ARCHIVE_WRITE(archive, entry.first);
            WG_ARCHIVE_WRITE(archive, entry.second);
        }
        return true;
    }
    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    bool archive_write(Archive& archive, const T& enum_value) {
        int value = static_cast<int>(enum_value);
        WG_ARCHIVE_WRITE(archive, value);
        return true;
    }

    template<typename T, std::size_t S>
    bool archive_read(Archive& archive, std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            WG_ARCHIVE_READ(archive, array[i]);
        }
        return true;
    }
    template<typename T>
    bool archive_read(Archive& archive, std::vector<T>& vector) {
        assert(vector.empty());
        std::size_t size;
        WG_ARCHIVE_READ(archive, size);
        vector.resize(size);
        for (int i = 0; i < size; i++) {
            WG_ARCHIVE_READ(archive, vector[i]);
        }
        return true;
    }
    template<typename K, typename V>
    bool archive_read(Archive& archive, std::unordered_map<K, V>& map) {
        assert(map.empty());
        std::size_t size;
        WG_ARCHIVE_READ(archive, size);
        for (int i = 0; i < size; i++) {
            std::pair<K, V> entry;
            WG_ARCHIVE_READ(archive, entry.first);
            WG_ARCHIVE_READ(archive, entry.second);
            map.insert(std::move(entry));
        }
        return true;
    }
    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    bool archive_read(Archive& archive, T& enum_value) {
        int value;
        WG_ARCHIVE_READ(archive, value);
        enum_value = static_cast<T>(value);
        return true;
    }

    template<typename T>
    Archive& operator<<(Archive& archive, const T& value) {
        const bool status = archive_write(archive, value);
        if (!status) {
            assert(status);
            WG_LOG_ERROR("failed to write value to archive " << archive.get_name());
        }
        return archive;
    }
    template<typename T>
    Archive& operator>>(Archive& archive, T& value) {
        const bool status = archive_read(archive, value);
        if (!status) {
            assert(status);
            WG_LOG_ERROR("failed to write value to archive " << archive.get_name());
        }
        return archive;
    }

}// namespace wmoge

#endif//WMOGE_ARCHIVE_HPP

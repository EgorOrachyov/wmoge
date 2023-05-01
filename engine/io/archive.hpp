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

#include <array>
#include <cinttypes>
#include <unordered_map>
#include <vector>

#include "core/ref.hpp"
#include "core/string_id.hpp"

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

        virtual bool        is_memory()   = 0;
        virtual bool        is_physical() = 0;
        virtual std::size_t get_size()    = 0;

        bool            can_read() const { return m_can_read; }
        bool            can_write() const { return m_can_write; }
        const StringId& get_name() const { return m_name; }

        friend Archive& operator<<(Archive& archive, const bool& value);
        friend Archive& operator<<(Archive& archive, const int& value);
        friend Archive& operator<<(Archive& archive, const float& value);
        friend Archive& operator<<(Archive& archive, const std::size_t& value);
        friend Archive& operator<<(Archive& archive, const StringId& value);
        friend Archive& operator<<(Archive& archive, const std::string& value);

        friend Archive& operator>>(Archive& archive, bool& value);
        friend Archive& operator>>(Archive& archive, int& value);
        friend Archive& operator>>(Archive& archive, float& value);
        friend Archive& operator>>(Archive& archive, std::size_t& value);
        friend Archive& operator>>(Archive& archive, StringId& value);
        friend Archive& operator>>(Archive& archive, std::string& value);

        template<typename T, std::size_t S>
        friend Archive& operator<<(Archive& archive, const std::array<T, S>& array);
        template<typename T>
        friend Archive& operator<<(Archive& archive, const std::vector<T>& vector);
        template<typename K, typename V>
        friend Archive& operator<<(Archive& archive, const std::unordered_map<K, V>& map);

        template<typename T, std::size_t S>
        friend Archive& operator>>(Archive& archive, std::array<T, S>& array);
        template<typename T>
        friend Archive& operator>>(Archive& archive, std::vector<T>& vector);
        template<typename K, typename V>
        friend Archive& operator>>(Archive& archive, std::unordered_map<K, V>& map);

    protected:
        StringId m_name;
        bool     m_can_read  = false;
        bool     m_can_write = false;
    };

    template<typename T, std::size_t S>
    Archive& operator<<(Archive& archive, const std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            archive << array[i];
        }
        return archive;
    }
    template<typename T>
    Archive& operator<<(Archive& archive, const std::vector<T>& vector) {
        archive << static_cast<int>(vector.size());
        for (const auto& entry : vector) {
            archive << entry;
        }
        return archive;
    }
    template<typename K, typename V>
    Archive& operator<<(Archive& archive, const std::unordered_map<K, V>& map) {
        archive << static_cast<int>(map.size());
        for (const auto& entry : map) {
            archive << entry.first;
            archive << entry.second;
        }
        return archive;
    }
    template<typename T, std::size_t S>
    Archive& operator>>(Archive& archive, std::array<T, S>& array) {
        for (std::size_t i = 0; i < S; i++) {
            archive >> array[i];
        }
        return archive;
    }
    template<typename T>
    Archive& operator>>(Archive& archive, std::vector<T>& vector) {
        assert(vector.empty());
        int size;
        archive >> size;
        vector.resize(size);
        for (int i = 0; i < size; i++) {
            archive >> vector[i];
        }
        return archive;
    }
    template<typename K, typename V>
    Archive& operator>>(Archive& archive, std::unordered_map<K, V>& map) {
        assert(map.empty());
        int size;
        archive >> size;
        for (int i = 0; i < size; i++) {
            std::pair<K, V> entry;
            archive >> entry.first;
            archive >> entry.second;
            map.insert(std::move(entry));
        }
        return archive;
    }

}// namespace wmoge

#endif//WMOGE_ARCHIVE_HPP

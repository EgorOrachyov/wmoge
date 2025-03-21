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

#include "core/string_id.hpp"
#include "core/flat_map.hpp"
#include "core/synchronization.hpp"

#include "string_id.hpp"
#include <cassert>
#include <memory>
#include <mutex>

namespace wmoge {

    /**
     * @class StringStorage
     * @brief Global storage to hold unique string ids data
     */
    class StringStorage {
    public:
        void                  get_or_create(const std::string& key, const std::string*& str);
        static StringStorage& instance(StridPool pool);

    private:
        struct Entry {
            std::string str;
        };

    private:
        flat_map<std::string, std::unique_ptr<Entry>> m_entries;
        mutable SpinMutex                             m_mutex;
    };

    void StringStorage::get_or_create(const std::string& key, const std::string*& str) {
        assert(!key.empty());

        std::lock_guard guard(m_mutex);
        auto&           entry = m_entries[key];

        if (entry == nullptr) {
            entry      = std::make_unique<Entry>();
            entry->str = key;
        }

        str = &entry->str;
    }

    StringStorage& StringStorage::instance(StridPool pool) {
        static StringStorage g_storage[static_cast<int>(StridPool::Max)];
        return g_storage[static_cast<int>(pool)];
    }

    Strid::Strid() {
        static std::string g_empty;
        m_string = &g_empty;
    }

    Strid::Strid(const char* string) : Strid(std::string(string), StridPool::Release) {
    }
    Strid::Strid(const std::string& string) : Strid(string, StridPool::Release) {
    }

    Strid::Strid(const std::string& string, StridPool pool) : Strid() {
        if (string.empty())
            return;
        StringStorage::instance(pool).get_or_create(string, m_string);
    }

    bool Strid::operator==(const Strid& other) const {
        return m_string == other.m_string;
    }

    bool Strid::operator!=(const Strid& other) const {
        return m_string != other.m_string;
    }

    bool Strid::operator<(const Strid& other) const {
        return *m_string < *other.m_string;
    }

    std::size_t Strid::id() const {
        return reinterpret_cast<std::size_t>(m_string);
    }

    std::size_t Strid::hash() const {
        return std::hash<std::size_t>()(id());
    }

    const std::string& Strid::str() const {
        return *m_string;
    }

}// namespace wmoge
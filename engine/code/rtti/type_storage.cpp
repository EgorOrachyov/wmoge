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

#include "type_storage.hpp"

#include <cassert>

namespace wmoge {

    RttiTypeStorage  g_storage_default;
    RttiTypeStorage* RttiTypeStorage::g_storage = &g_storage_default;

    std::optional<RttiType*> RttiTypeStorage::find_type(const Strid& name) {
        std::lock_guard guard(m_mutex);

        auto query = m_types.find(name);

        if (query != m_types.end()) {
            return query->second.get();
        }

        return std::nullopt;
    }

    RttiStruct* RttiTypeStorage::find_struct(const Strid& name) {
        return dynamic_cast<RttiStruct*>(find_type(name).value_or(nullptr));
    }

    RttiClass* RttiTypeStorage::find_class(const Strid& name) {
        return dynamic_cast<RttiClass*>(find_type(name).value_or(nullptr));
    }

    bool RttiTypeStorage::has(const Strid& name) {
        std::lock_guard guard(m_mutex);

        auto query = m_types.find(name);
        return query != m_types.end();
    }

    void RttiTypeStorage::add(const Ref<RttiType>& type) {
        std::lock_guard guard(m_mutex);

        assert(type);
        assert(m_types.find(type->get_name()) == m_types.end() || m_types[type->get_name()] == type);

        m_types[type->get_name()] = type;
    }

    std::vector<RttiType*> RttiTypeStorage::find_types(const std::function<bool(const Ref<RttiType>&)>& filter) {
        std::lock_guard guard(m_mutex);

        std::vector<RttiType*> result;
        for (auto& entry : m_types) {
            if (filter(entry.second)) {
                result.push_back(entry.second.get());
            }
        }
        return std::move(result);
    }

    std::vector<RttiClass*> RttiTypeStorage::find_classes(const std::function<bool(const Ref<RttiClass>&)>& filter) {
        std::lock_guard guard(m_mutex);

        std::vector<RttiClass*> result;
        for (auto& entry : m_types) {
            Ref<RttiClass> casted = entry.second.cast<RttiClass>();
            if (casted && filter(casted)) {
                result.push_back(casted.get());
            }
        }
        return std::move(result);
    }

    RttiTypeStorage* RttiTypeStorage::instance() {
        return g_storage;
    }

    void RttiTypeStorage::provide(RttiTypeStorage* storage) {
        g_storage = storage;
    }

}// namespace wmoge
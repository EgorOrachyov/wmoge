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

#include "ioc_container.hpp"

namespace wmoge {

    void IocContainer::clear() {
        std::lock_guard guard(m_mutex);

        m_entries.clear();
    }

    void IocContainer::add(IocEntry&& entry) {
        std::lock_guard guard(m_mutex);

        auto query = m_entries.find(entry.source_type.value());

        if (query != m_entries.end()) {
            WG_LOG_ERROR("attempt to re-bind type " << entry.source_type.value().name()
                                                    << " with " << entry.provided_type.value().name());
            return;
        }

        WG_LOG_INFO("bind '" << entry.source_type.value().name() << "'");
        m_entries[entry.source_type.value()] = std::move(entry);
    }

    void IocContainer::erase(std::type_index entry_type) {
        std::lock_guard guard(m_mutex);

        auto iter = m_entries.find(entry_type);
        if (iter != m_entries.end()) {
            m_entries.erase(iter);
        }
    }

    std::optional<IocEntry*> IocContainer::get(std::type_index entry_type) {
        std::lock_guard guard(m_mutex);

        auto query = m_entries.find(entry_type);

        if (query != m_entries.end()) {
            return std::optional(&(query->second));
        }

        return std::nullopt;
    }

    IocContainer* IocContainer::instance() {
        static IocContainer instance;
        return &instance;
    }

}// namespace wmoge
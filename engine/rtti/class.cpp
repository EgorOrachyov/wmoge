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

#include "class.hpp"

#include <cassert>
#include <cinttypes>

namespace wmoge {

    RttiClass::RttiClass(Strid name, std::size_t byte_size, RttiClass* parent)
        : RttiStruct(name, byte_size, parent) {
        m_parent_class = parent;

        if (parent) {
            m_methods     = parent->m_methods;
            m_methods_map = parent->m_methods_map;
            m_signals     = parent->m_signals;
            m_signals_map = parent->m_signals_map;
        }
    }

    std::optional<const RttiMethod*> RttiClass::find_method(const Strid& name) const {
        auto query = m_methods_map.find(name);

        if (query != m_methods_map.end()) {
            return &m_methods[query->second];
        }

        return std::nullopt;
    }

    void RttiClass::add_method(RttiMethod method) {
        assert(!has_method(method.get_name()));

        const std::int16_t id = std::int16_t(m_methods.size());
        m_methods.push_back(std::move(method));
        m_methods_map[m_methods.back().get_name()] = id;
        m_members.insert(m_methods.back().get_name());
    }

    bool RttiClass::has_method(const Strid& name) const {
        auto query = m_methods_map.find(name);
        return query != m_methods_map.end();
    }

    std::optional<const RttiSignal*> RttiClass::find_signal(const Strid& name) const {
        auto query = m_signals_map.find(name);

        if (query != m_signals_map.end()) {
            return &m_signals[query->second];
        }

        return std::nullopt;
    }

    void RttiClass::add_signal(RttiSignal signal) {
        assert(!has_signal(signal.get_name()));

        const std::int16_t id = std::int16_t(m_signals.size());
        m_signals.push_back(std::move(signal));
        m_signals_map[m_signals.back().get_name()] = id;
        m_members.insert(m_signals.back().get_name());
    }

    bool RttiClass::has_signal(const Strid& name) const {
        auto query = m_signals_map.find(name);
        return query != m_signals_map.end();
    }

    void RttiClass::add_factory(std::function<class RttiObject*()> factory) {
        m_factory = std::move(factory);
    }

    bool RttiClass::can_instantiate() const {
        return m_factory.operator bool();
    }

    class RttiObject* RttiClass::instantiate() const {
        return m_factory ? m_factory() : nullptr;
    }

}// namespace wmoge
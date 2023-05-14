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

#include "event_manager.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"

#include <cassert>

namespace wmoge {

    void EventManager::subscribe(const Ref<EventListener>& listener) {
        assert(listener);
        assert(!listener->connected());

        if (!listener) {
            WG_LOG_ERROR("passed null listener");
            return;
        }

        if (listener->connected()) {
            WG_LOG_ERROR("passed listener already connected");
            return;
        }

        std::lock_guard guard(m_mutex);
        listener->m_connected = true;
        m_pending_add.push_back(listener);
    }

    void EventManager::unsubscribe(const Ref<EventListener>& listener) {
        assert(listener);
        assert(listener->connected());

        if (!listener) {
            WG_LOG_ERROR("passed null listener");
            return;
        }

        if (!listener->connected()) {
            WG_LOG_ERROR("attempt to unsubscribe unconnected listener");
            return;
        }

        std::lock_guard guard(m_mutex);
        listener->m_connected = false;
        m_pending_remove.push_back(listener);
    }

    void EventManager::dispatch(const Ref<Event>& event) {
        assert(event);

        if (!event) {
            WG_LOG_ERROR("passed null event");
            return;
        }

        std::lock_guard guard(m_mutex);
        m_events.push_back(event);
    }

    void EventManager::update() {
        WG_AUTO_PROFILE_CORE("EventManager::update");

        fast_vector<Ref<EventListener>> to_add;
        fast_vector<Ref<EventListener>> to_remove;
        fast_vector<Ref<Event>>         to_dispatch;
        {
            std::lock_guard guard(m_mutex);
            std::swap(to_add, m_pending_add);
            std::swap(to_remove, m_pending_remove);
            std::swap(to_dispatch, m_events);
        }

        for (auto& listener : to_add)
            m_listeners[listener->type()].push_back(listener);

        for (auto& listener : to_remove) {
            auto& list      = m_listeners[listener->type()];
            auto  remove_it = std::find(list.begin(), list.end(), listener);
            assert(remove_it != list.end());
            list.erase(remove_it);
        }

        for (auto& event : to_dispatch) {
            auto query = m_listeners.find(event->type());
            if (query != m_listeners.end()) {
                for (auto& listener : query->second) {
                    assert(listener->connected());
                    if (!listener->paused()) {
                        // Returns true, we must stop propagation
                        if (listener->on_event(event))
                            break;
                    }
                }
            }
        }
    }
    void EventManager::shutdown() {
        WG_AUTO_PROFILE_CORE("EventManager::shutdown");

        m_listeners.clear();
        m_pending_add.clear();
        m_pending_remove.clear();
        m_events.clear();
    }

}// namespace wmoge

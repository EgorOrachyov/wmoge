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

    EventManager::EventManager() : m_listeners_pool(sizeof(EventListener)) {
    }

    EventManager::~EventManager() {
#ifdef WG_DEBUG
        if (!m_hnd_to_listener.empty()) {
            WG_LOG_INFO("explicitly unsubscribed count=" << m_hnd_to_listener.size());
        }
#endif

        for (auto [hnd, listener] : m_hnd_to_listener) {
            listener->~EventListener();
            m_listeners_pool.free(listener);
        }

        m_event_to_listener.clear();
        m_hnd_to_listener.clear();
    }

    EventListenerHnd EventManager::subscribe(const EventType& event_type, EventCallback callback) {
        WG_AUTO_PROFILE_CORE("EventManager::subscribe");

        std::lock_guard guard(m_mutex);

        assert(callback);
        assert(!event_type.empty());

        if (m_dispatching) {
            WG_LOG_ERROR("cannot do operations within call chain");
            return {};
        }

        auto* listener     = new (m_listeners_pool.allocate()) EventListener;
        listener->callback = std::move(callback);
        listener->type     = event_type;
        listener->hnd      = m_next_hnd;
        listener->paused   = false;

        m_next_hnd.value += 1;

        m_event_to_listener[listener->type].push_back(listener);
        m_hnd_to_listener[listener->hnd] = listener;

        return listener->hnd;
    }

    void EventManager::unsubscribe(EventListenerHnd hnd) {
        WG_AUTO_PROFILE_CORE("EventManager::unsubscribe");

        std::lock_guard guard(m_mutex);

        assert(hnd.is_valid());
        assert(m_hnd_to_listener.find(hnd) != m_hnd_to_listener.end());

        if (m_dispatching) {
            WG_LOG_ERROR("cannot do operations within call chain");
            return;
        }

        auto query = m_hnd_to_listener.find(hnd);
        if (query != m_hnd_to_listener.end()) {
            auto* listener = query->second;
            m_event_to_listener.erase(listener->type);
            m_hnd_to_listener.erase(query);
            listener->~EventListener();
            m_listeners_pool.free(listener);
        }
    }

    void EventManager::dispatch(const Ref<Event>& event) {
        WG_AUTO_PROFILE_CORE("EventManager::dispatch");

        std::lock_guard guard(m_mutex);

        assert(event);
        assert(!event->type().empty());

        const bool dispatching = m_dispatching;
        m_dispatching          = true;

        auto listeners_list = m_event_to_listener.find(event->type());
        if (listeners_list != m_event_to_listener.end()) {
            for (auto& listener : listeners_list->second) {
                if (listener->callback(event)) {
                    break;
                }
            }
        }

        m_dispatching = dispatching;
    }

    void EventManager::dispatch_deferred(const Ref<Event>& event) {
        WG_AUTO_PROFILE_CORE("EventManager::dispatch_deferred");

        std::lock_guard guard(m_mutex);

        assert(event);
        assert(!event->type().empty());

        m_events_deferred.push_back(event);
    }

    void EventManager::flush() {
        WG_AUTO_PROFILE_CORE("EventManager::flush");

        std::lock_guard guard(m_mutex);

        m_dispatching = true;

        buffered_vector<Ref<Event>> events;
        std::swap(events, m_events_deferred);

        for (auto& event : events) {
            auto listeners_list = m_event_to_listener.find(event->type());
            if (listeners_list != m_event_to_listener.end()) {
                for (auto& listener : listeners_list->second) {
                    if (listener->callback(event)) {
                        break;
                    }
                }
            }
        }

        m_dispatching = false;
    }

}// namespace wmoge

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

#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "event/event.hpp"
#include "event/event_listener.hpp"
#include "memory/mem_pool.hpp"

#include <functional>
#include <list>
#include <mutex>

namespace wmoge {

    /**
     * @class EventManager
     * @brief Event manager is responsible for engine events
     *
     *  Use this class to:
     *  - Subscribe listeners to events
     *  - Dispatch events and notify listeners
     */
    class EventManager {
    public:
        EventManager();
        ~EventManager();

        /**
         * @brief Subscribes custom listener with specific event type
         *
         * @tparam E Type of event to listen to
         * @tparam C Type of function to bind
         *
         * @param callable Function to subscribe
         *
         * @return Handle to a listener
         */
        template<typename E, typename C>
        EventListenerHnd subscribe(C&& callable);

        /**
         * @brief Subscribes listener to an event
         *
         * @param event_type Name of type of event to listen to
         * @param callback Function to subscribe
         *
         * @return Handle to a listener
         */
        EventListenerHnd subscribe(const EventType& event_type, EventCallback callback);

        /**
         * @brief Unsubscribes listener
         *
         * @param hnd Handle to listener
         */
        void unsubscribe(EventListenerHnd hnd);

        /**
         * @brief Dispatch immediately event
         *
         * @param event Event to dispatch
         */
        void dispatch(const Ref<Event>& event);

        /**
         * @brief Queue event to dispatch
         *
         * @param event Event to dispatch later
         */
        void dispatch_deferred(const Ref<Event>& event);

        /**
         * @brief Dispatch queued events
         */
        void flush();

    private:
        flat_map<EventType, buffered_vector<EventListener*>> m_event_to_listener;
        flat_map<EventListenerHnd, EventListener*>           m_hnd_to_listener;
        buffered_vector<Ref<Event>>                          m_events;
        buffered_vector<Ref<Event>>                          m_events_deferred;

        MemPool          m_listeners_pool;
        EventListenerHnd m_next_hnd{0};
        bool             m_dispatching = false;

        mutable std::recursive_mutex m_mutex;
    };

    template<typename E, typename C>
    EventListenerHnd EventManager::subscribe(C&& callable) {
        EventCallback callback = [c = std::forward<C>(callable)](const Ref<Event>& event) {
            const E* casted_event = dynamic_cast<const E*>(event.get());
            if (!casted_event) {
                WG_LOG_ERROR("failed to cast event to expected type");
                return false;
            }
            return c(*casted_event);
        };
        return subscribe(E::type_static(), std::move(callback));
    }

}// namespace wmoge
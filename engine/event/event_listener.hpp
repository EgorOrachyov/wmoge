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

#ifndef WMOGE_EVENT_LISTENER_HPP
#define WMOGE_EVENT_LISTENER_HPP

#include "core/log.hpp"
#include "event/event.hpp"

#include <functional>

namespace wmoge {

    /**
     * @class EventCallback
     * @brief Listener function called on event
     */
    using EventCallback = std::function<bool(const Ref<Event>& event)>;

    /**
     * @class EventListener
     * @brief Particular event listener with action to perform on event
     */
    class EventListener : public RefCnt {
    public:
        EventListener(EventType event_type, EventCallback callback, class Object* target = nullptr);
        ~EventListener() override = default;

        virtual bool on_event(const Ref<Event>& event);

        void unsubscribe();
        void pause();
        void resume();

        [[nodiscard]] const EventType& type() const { return m_event_type; }
        [[nodiscard]] class Object*    target() const { return m_target; }
        [[nodiscard]] bool             paused() const { return m_paused; }
        [[nodiscard]] bool             connected() const { return m_connected; }

    private:
        friend class EventManager;
        friend class Object;

        EventCallback m_callback;
        EventType     m_event_type;
        class Object* m_target;
        bool          m_paused    = false;
        bool          m_connected = false;
    };

    /**
     * @brief Makes new event listener from method bind
     *
     * @tparam E Type of event
     * @tparam C Type of object to call
     *
     * @param callable Callable object to process event
     *
     * @return New listener
     */
    template<typename E, typename C>
    inline Ref<EventListener> make_listener(C callable) {
        EventCallback callback = [c = std::move(callable)](const Ref<Event>& event) {
            const E* casted_event = dynamic_cast<const E*>(event.get());
            if (!casted_event) {
                WG_LOG_ERROR("failed to cast event to expected type");
                return false;
            }
            return c(*casted_event);
        };
        return make_ref<EventListener>(E::type_static(), std::move(callback));
    }

    /**
     * @brief Makes new event listener from object method bind
     *
     * @tparam T Type of object to bind
     * @tparam E Type of event
     *
     * @param target Object instance to bind
     * @param method_pointer Object method
     *
     * @return New listener
     */
    template<typename T, typename E>
    inline Ref<EventListener> make_listener(T* target, bool (T::*method_pointer)(const E&)) {
        EventCallback callback = [target, method_pointer](const Ref<Event>& event) {
            const E* casted_event = dynamic_cast<const E*>(event.get());
            if (!casted_event) {
                WG_LOG_ERROR("failed to cast event to expected type");
                return false;
            }
            return (*target.*method_pointer)(*casted_event);
        };
        return make_ref<EventListener>(E::type_static(), std::move(callback), target);
    }

}// namespace wmoge

#endif//WMOGE_EVENT_LISTENER_HPP

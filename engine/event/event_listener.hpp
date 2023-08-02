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
#include "core/string_utils.hpp"
#include "event/event.hpp"

#include <cinttypes>
#include <functional>

namespace wmoge {

    /**
     * @brief Listener function called on event
     */
    using EventCallback = std::function<bool(const Ref<Event>& event)>;

    /**
     * @class EventListenerHnd
     * @brief Handle to track event listener
     */
    struct EventListenerHnd {
        std::int32_t value = -1;

        bool operator==(const EventListenerHnd& other) const { return value == other.value; }
        bool operator!=(const EventListenerHnd& other) const { return value != other.value; }

        [[nodiscard]] bool is_valid() const { return value != -1; }
        [[nodiscard]] bool is_invalid() const { return value == -1; }

        [[nodiscard]] std::string to_string() const { return StringUtils::from_int(value); }
    };

    static_assert(sizeof(EventListenerHnd) == sizeof(std::int32_t), "Must fit 32bit word");

    /**
     * @class EventListener
     * @brief Struct holding info about a particular listener
     */
    struct EventListener {
        EventCallback    callback;
        EventType        type;
        EventListenerHnd hnd;
        bool             paused = false;
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::EventListenerHnd> {
    public:
        std::size_t operator()(const wmoge::EventListenerHnd& hnd) const {
            return std::hash<int>()(hnd.value);
        }
    };

}// namespace std

#endif//WMOGE_EVENT_LISTENER_HPP

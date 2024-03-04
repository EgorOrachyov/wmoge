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

#include "core/class.hpp"
#include "core/object.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"

#include <string>

namespace wmoge {

    /**
     * @brief Type to identify different events
     */
    using EventType = Strid;

    /**
     * @class Event
     * @brief Event holding some data which can be processed by listeners
     */
    class Event : public Object {
    public:
        WG_OBJECT(Event, Object)

        virtual const EventType& type() const {
            static Strid none;
            return none;
        };
    };

    template<typename T, typename... TArgs>
    Ref<T> make_event(TArgs&&... args) {
        return make_ref<T>(std::forward<TArgs>(args)...);
    }

}// namespace wmoge

// Use this macro to declare basic methods for your custom event type.
// This macro adds type and static type methods version to get event type.
#define WG_EVENT(event_class, event_class_base) \
    WG_OBJECT(event_class, event_class_base)    \
    const EventType& type() const override {    \
        return type_static();                   \
    }                                           \
    static const EventType& type_static() {     \
        static auto g_id = SID(#event_class);   \
        return g_id;                            \
    }                                           \
    std::string to_string() override;

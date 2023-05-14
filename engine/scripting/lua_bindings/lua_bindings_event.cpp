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

#include "lua_bindings_event.hpp"

#include "scripting/lua/lua_event_script.hpp"
#include "scripting/lua/lua_script_system.hpp"

namespace wmoge {

    static int lua_EventManager_subscribe(lua_State* state) {
        auto arg_type     = WG_LUA_ARG(state, 0);
        auto arg_function = WG_LUA_ARG(state, 1);

        if (!arg_type.isString() && !arg_type.isInstance<StringId>()) {
            WG_LOG_ERROR("invalid event type " << arg_type.tostring());
            return 0;
        }

        if (!arg_function.isFunction()) {
            WG_LOG_ERROR("expected function to subscribe");
            return 0;
        }

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();
        auto* script_system = dynamic_cast<LuaScriptSystem*>(engine->script_system());

        auto event_type = LuaTypeTraits::to_sid(arg_type);
        auto listener   = make_ref<EventListener>(event_type, [script_system, arg_function](const Ref<Event>& event) {
            std::lock_guard guard(script_system->get_mutex());

            if (LuaEventScript* lua_event = dynamic_cast<LuaEventScript*>(event.get())) {
                if (lua_event->data.has_value()) {
                    arg_function(lua_event->data.value());
                }
                return false;
            }

            arg_function(LuaTypeTraits::from_object(arg_function.state(), event.get()));
            return false;
        });

        event_manager->subscribe(listener);

        luabridge::LuaRef(state, LuaEventListener{listener}).push();
        return 1;
    }

    static int lua_EventManager_dispatch(lua_State* state) {
        auto arg_type = WG_LUA_ARG(state, 0);
        auto arg_data = WG_LUA_ARG(state, 1);

        if (!arg_type.isString() && !arg_type.isInstance<StringId>()) {
            WG_LOG_ERROR("invalid event type " << arg_type.tostring());
            return 0;
        }

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        auto event = make_event<LuaEventScript>();
        event->set_type(std::move(LuaTypeTraits::to_sid(arg_type)));
        event->data.emplace(std::move(arg_data));

        event_manager->dispatch(event);
        return 0;
    }

    void LuaBindings::bind_event(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping) {
        bind_enum<WindowNotification>(ns, "WindowNotification");
        bind_enum<ResourceNotification>(ns, "ResourceNotification");
        bind_enum<TokenNotification>(ns, "TokenNotification");

        ns = ns.beginNamespace("EventManager")
                     .addFunction("subscribe", lua_EventManager_subscribe)
                     .addFunction("dispatch", lua_EventManager_dispatch)
                     .endNamespace();

        ns = ns.deriveClass<LuaEventListener, LuaRefCnt>("EventListener")
                     .addFunction("unsubscribe", &LuaEventListener::unsubscribe)
                     .addFunction("pause", &LuaEventListener::pause)
                     .addFunction("resume", &LuaEventListener::resume)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Event);
        ns = ns.deriveClass<LuaEvent, LuaObject>("Event")
                     .addFunction("type", &LuaEvent::type)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventWindow);
        ns = ns.deriveClass<LuaEventWindow, LuaEvent>("EventWindow")
                     .addProperty("notification", &LuaEventWindow::notification)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventInput);
        ns = ns.deriveClass<LuaEventInput, LuaEvent>("EventInput")
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventMouse);
        ns = ns.deriveClass<LuaEventMouse, LuaEventInput>("EventMouse")
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventKeyboard);
        ns = ns.deriveClass<LuaEventKeyboard, LuaEventInput>("EventKeyboard")
                     .addProperty("action", &LuaEventKeyboard::action)
                     .addProperty("key", &LuaEventKeyboard::key)
                     .addProperty("text", &LuaEventKeyboard::text)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventJoystick);
        ns = ns.deriveClass<LuaEventJoystick, LuaEventInput>("EventJoystick")
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventDrop);
        ns = ns.deriveClass<LuaEventDrop, LuaEventInput>("EventDrop")
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventAction);
        ns = ns.deriveClass<LuaEventAction, LuaEvent>("EventAction")
                     .addProperty("name", &LuaEventAction::name)
                     .addProperty("strength", &LuaEventAction::strength)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, EventToken);
        ns = ns.deriveClass<LuaEventToken, LuaEvent>("EventToken")
                     .addProperty("token", &LuaEventToken::token)
                     .addProperty("notification", &LuaEventToken::notification)
                     .endClass();
    }

}// namespace wmoge
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

#include "lua_script_instance.hpp"

#include "debug/profiler.hpp"
#include "scripting/lua/lua_script.hpp"
#include "scripting/lua_bindings/lua_bindings_event.hpp"

namespace wmoge {

#define WG_SAFE_CALL(func, name, ...)                         \
    try {                                                     \
        std::lock_guard guard(m_system->get_mutex());         \
        if (!m_mask.get(func)) return;                        \
        if (m_mask_failed.get(func)) return;                  \
                                                              \
        auto callback = m_script_object[name];                \
        assert(callback.isFunction());                        \
                                                              \
        if (!callback.isFunction()) {                         \
            m_mask_failed.set(func);                          \
            WG_LOG_ERROR("expected function to call " #func); \
        }                                                     \
                                                              \
        callback(__VA_ARGS__);                                \
    } catch (const std::exception& e) {                       \
        m_mask_failed.set(func);                              \
        WG_LOG_ERROR("lua error: " << e.what());              \
    }

    LuaScriptInstance::LuaScriptInstance(luabridge::LuaRef script_object, Ref<LuaScript> script, Object* object, lua_State* state)
        : m_script_object(std::move(script_object)),
          m_script(std::move(script)),
          m_object(object),
          m_state(state),
          m_mask(m_script->get_mask()) {
        m_system = m_script->get_system();
    }
    LuaScriptInstance::~LuaScriptInstance() = default;

    ScriptFunctionsMask LuaScriptInstance::get_mask() {
        return m_mask;
    }
    Script* LuaScriptInstance::get_script() {
        return m_script.get();
    }
    Object* LuaScriptInstance::get_owner() {
        return m_object;
    }
    void LuaScriptInstance::on_create() {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_create");
        WG_SAFE_CALL(ScriptFunction::OnCreate, "on_create", m_script_object);
    }
    void LuaScriptInstance::on_scene_enter() {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_scene_enter");
        WG_SAFE_CALL(ScriptFunction::OnSceneEnter, "on_scene_enter", m_script_object);
    }
    void LuaScriptInstance::on_scene_exit() {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_scene_exit");
        WG_SAFE_CALL(ScriptFunction::OnSceneExit, "on_scene_exit", m_script_object);
    }
    void LuaScriptInstance::on_transform_updated() {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_transform_updated");
        WG_SAFE_CALL(ScriptFunction::OnTransformUpdated, "on_transform_updated", m_script_object);
    }
    void LuaScriptInstance::on_update(float delta_time) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_update");
        WG_SAFE_CALL(ScriptFunction::OnUpdate, "on_update", m_script_object, delta_time);
    }
    void LuaScriptInstance::on_signal(const Strid& signal) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_signal");
        WG_SAFE_CALL(ScriptFunction::OnSignal, "on_signal", m_script_object, luabridge::LuaRef(m_state, signal));
    }
    void LuaScriptInstance::on_input_mouse(const Ref<EventMouse>& event) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_input_mouse");
        WG_SAFE_CALL(ScriptFunction::OnInputMouse, "on_input_mouse", m_script_object, luabridge::LuaRef(m_state, LuaEventMouse{Ref<RefCnt>(event.get())}));
    }
    void LuaScriptInstance::on_input_keyboard(const Ref<EventKeyboard>& event) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_input_keyboard");
        WG_SAFE_CALL(ScriptFunction::OnInputKeyboard, "on_input_keyboard", m_script_object, luabridge::LuaRef(m_state, LuaEventKeyboard{Ref<RefCnt>(event.get())}));
    }
    void LuaScriptInstance::on_input_joystick(const Ref<EventJoystick>& event) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_input_joystick");
        WG_SAFE_CALL(ScriptFunction::OnInputJoystick, "on_input_joystick", m_script_object, luabridge::LuaRef(m_state, LuaEventJoystick{Ref<RefCnt>(event.get())}));
    }
    void LuaScriptInstance::on_input_drop(const Ref<EventDrop>& event) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_input_drop");
        WG_SAFE_CALL(ScriptFunction::OnInputDrop, "on_input_drop", m_script_object, luabridge::LuaRef(m_state, LuaEventDrop{Ref<RefCnt>(event.get())}));
    }
    void LuaScriptInstance::on_action(const Ref<EventAction>& action) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_action");
        WG_SAFE_CALL(ScriptFunction::OnAction, "on_action", m_script_object, luabridge::LuaRef(m_state, LuaEventAction{Ref<RefCnt>(action.get())}));
    }
    void LuaScriptInstance::on_token(const Ref<EventToken>& token) {
        WG_AUTO_PROFILE_LUA("LuaScriptInstance::on_token");
        WG_SAFE_CALL(ScriptFunction::OnToken, "on_token", m_script_object, luabridge::LuaRef(m_state, LuaEventToken{Ref<RefCnt>(token.get())}));
    }
    int LuaScriptInstance::set(const Strid& property, const Var& value) {
        return ScriptInstance::set(property, value);
    }
    int LuaScriptInstance::get(const Strid& property, Var& value) {
        return ScriptInstance::get(property, value);
    }
    int LuaScriptInstance::call(const Strid& method, int argc, const Var* argv, Var& ret) {
        return ScriptInstance::call(method, argc, argv, ret);
    }

}// namespace wmoge

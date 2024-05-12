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

#ifndef WMOGE_SCRIPT_INSTANCE_HPP
#define WMOGE_SCRIPT_INSTANCE_HPP

#include "core/mask.hpp"
#include "core/object.hpp"
#include "core/ref.hpp"
#include "core/var.hpp"
#include "event/event_action.hpp"
#include "event/event_input.hpp"
#include "event/event_token.hpp"

namespace wmoge {

    /**
     * @class ScriptFunction
     * @brief On-event function exposed by script
     */
    enum class ScriptFunction {
        OnCreate,
        OnSceneEnter,
        OnSceneExit,
        OnTransformUpdated,
        OnUpdate,
        OnSignal,
        OnInputMouse,
        OnInputKeyboard,
        OnInputJoystick,
        OnInputDrop,
        OnAction,
        OnToken,
        Total
    };

    /**
     * @class ScriptFunctionsMask
     * @brief Mask with script defined functions
     */
    using ScriptFunctionsMask = Mask<ScriptFunction>;

    /**
     * @class ScriptInstance
     * @brief An instance of the script which can control an object logic
     */
    class ScriptInstance : public RefCnt {
    public:
        ~ScriptInstance() override = default;

        virtual ScriptFunctionsMask get_mask() { return ScriptFunctionsMask(); };
        virtual class Script*       get_script() { return nullptr; };
        virtual class Object*       get_owner() { return nullptr; };

        virtual void on_create() {}
        virtual void on_scene_enter() {}
        virtual void on_scene_exit() {}
        virtual void on_transform_updated() {}
        virtual void on_update(float delta_time){};
        virtual void on_signal(const Strid& signal) {}
        virtual void on_input_mouse(const Ref<EventMouse>& event) {}
        virtual void on_input_keyboard(const Ref<EventKeyboard>& event) {}
        virtual void on_input_joystick(const Ref<EventJoystick>& event) {}
        virtual void on_input_drop(const Ref<EventDrop>& event) {}
        virtual void on_action(const Ref<EventAction>& action) {}
        virtual void on_token(const Ref<EventToken>& token) {}

        virtual int set(const Strid& property, const Var& value) { return -2; }
        virtual int get(const Strid& property, Var& value) { return -2; }
        virtual int call(const Strid& method, int argc, const Var* argv, Var& ret) { return -2; }
    };

}// namespace wmoge

#endif//WMOGE_SCRIPT_INSTANCE_HPP

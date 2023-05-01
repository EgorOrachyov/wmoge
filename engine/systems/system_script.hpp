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

#ifndef WMOGE_SYSTEM_SCRIPT_HPP
#define WMOGE_SYSTEM_SCRIPT_HPP

#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "event/event_listener.hpp"
#include "scene/scene_system.hpp"

namespace wmoge {

    /**
     * @class SystemScript
     * @brief Process scene scripts
     */
    class SystemScript final : public SceneSystem {
    public:
        SystemScript(class Scene* scene);
        ~SystemScript() override;

        void process() override;

    private:
        fast_vector<ref_ptr<Event>> m_events;
        ref_ptr<EventListener>      m_on_action;
        ref_ptr<EventListener>      m_on_input_mouse;
        ref_ptr<EventListener>      m_on_input_keyboard;
        ref_ptr<EventListener>      m_on_input_joystick;
        ref_ptr<EventListener>      m_on_input_drop;
        ref_ptr<EventListener>      m_on_token;
    };

}// namespace wmoge

#endif//WMOGE_SYSTEM_SCRIPT_HPP

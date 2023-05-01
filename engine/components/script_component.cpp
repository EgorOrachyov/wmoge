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

#include "script_component.hpp"

#include "core/engine.hpp"
#include "resource/resource_manager.hpp"
#include "resource/script.hpp"
#include "scene/scene.hpp"
#include "scene/scene_object.hpp"
#include "scripting/script_instance.hpp"
#include "systems/system_script.hpp"

namespace wmoge {

    bool ScriptComponent::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneComponent::on_load_from_yaml(node)) {
            return false;
        }

        auto file   = Yaml::read_sid(node["script"]);
        auto script = Engine::instance()->resource_manager()->load(file).cast<Script>();
        if (!script) {
            WG_LOG_ERROR("failed to load script " << script);
            return false;
        }

        attach(std::move(script));
        return true;
    }
    void ScriptComponent::on_signal(const StringId& signal) {
        if (m_script) m_script->on_signal(signal);
    }

    void ScriptComponent::on_scene_enter() {
        SceneComponent::on_scene_enter();

        if (m_script) {
            m_script->on_scene_enter();
        }
    }

    void ScriptComponent::on_scene_exit() {
        SceneComponent::on_scene_exit();

        if (m_script) {
            m_script->on_scene_exit();
        }
    }

    void ScriptComponent::on_transform_updated() {
        if (m_script) m_script->on_transform_updated();
    }

    void ScriptComponent::attach(ref_ptr<Script> script) {
        assert(script);
        assert(!m_script);

        if (!script) {
            WG_LOG_ERROR("an attempt to attach null script");
            return;
        }
        if (m_script) {
            WG_LOG_ERROR("an attempt to attach script to object which already has a script");
            return;
        }

        m_script = script->attach_to(get_scene_object());
        m_script->on_create();

        if (is_in_scene()) {
            on_scene_enter();
        }
    }
    const ref_ptr<class ScriptInstance>& ScriptComponent::get_script() const {
        return m_script;
    }

}// namespace wmoge
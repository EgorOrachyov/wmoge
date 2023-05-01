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

#ifndef WMOGE_SCRIPT_COMPONENT_HPP
#define WMOGE_SCRIPT_COMPONENT_HPP

#include "scene/scene_component.hpp"
#include "scripting/script_instance.hpp"

namespace wmoge {

    /**
     * @class ScriptComponent
     * @brief Script which can be attached to a scene object to extend its logic
     */
    class ScriptComponent final : public SceneComponent {
    public:
        WG_OBJECT(ScriptComponent, SceneComponent)

        /** @brief Attach script to this component making new script instance */
        void                           attach(ref_ptr<class Script> script);
        const ref_ptr<ScriptInstance>& get_script() const;

        bool on_load_from_yaml(const YamlConstNodeRef& node) override;
        void on_signal(const StringId& signal);
        void on_scene_enter() override;
        void on_scene_exit() override;
        void on_transform_updated() override;

    private:
        ref_ptr<ScriptInstance> m_script;
    };

}// namespace wmoge

#endif//WMOGE_SCRIPT_COMPONENT_HPP

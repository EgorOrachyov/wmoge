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

#ifndef WMOGE_SCENE_HPP
#define WMOGE_SCENE_HPP

#include "core/class.hpp"
#include "core/fast_vector.hpp"
#include "pfx/pfx_scene.hpp"
#include "platform/window.hpp"
#include "render/render_engine.hpp"
#include "render/render_pipeline.hpp"
#include "render/render_scene.hpp"
#include "scene/scene_component.hpp"
#include "scene/scene_object.hpp"
#include "scene/scene_registry.hpp"
#include "systems/system_script.hpp"

namespace wmoge {

    /**
     * @class Scene
     * @brief Scene objects container representing virtual logical game scene
     */
    class Scene final : public Object {
    public:
        WG_OBJECT(Scene, Object);

        void init();
        void shutdown();

        void               add_child(Ref<SceneObject> object);
        class SceneObject* get_child(int idx);
        class SceneObject* find_child(const StringId& name);
        const StringId&    get_name();
        SceneRegistry*     get_registry();
        RenderScene*       get_render_scene();
        RenderPipeline*    get_render_pipeline();
        PfxScene*          get_pfx_scene();
        SystemScript*      get_system_script();

    private:
        friend class ScenePacked;
        friend class SceneManager;

        std::unique_ptr<SceneRegistry>  m_registry;
        std::unique_ptr<RenderScene>    m_render_scene;
        std::unique_ptr<RenderPipeline> m_render_pipeline;
        std::unique_ptr<PfxScene>       m_pfx_scene;
        std::unique_ptr<SystemScript>   m_system_script;

        Ref<SceneObject> m_root;
        StringId         m_name;
        float            m_time    = 0.0f;
        float            m_time_dt = 0.0f;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_HPP

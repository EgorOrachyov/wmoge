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

#ifndef WMOGE_RENDER_OBJECT_HPP
#define WMOGE_RENDER_OBJECT_HPP

#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "render/draw_cmd.hpp"
#include "render/render_view.hpp"

namespace wmoge {

    /**
     * @class RenderObject
     * @brief Base class for any readable object which can be added to the scene
     */
    class RenderObject : public RefCnt {
    public:
        ~RenderObject() override = default;

        /* render engine side functions */
        virtual void on_scene_enter() {}
        virtual void on_scene_exit() {}
        virtual void on_view_setup(RenderView* view) {}
        virtual void on_update(float dt) {}
        virtual void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) {}
        virtual bool need_update() const { return false; }
        virtual bool need_render_dynamic() const { return false; }
        virtual bool need_view() const { return false; }

        /* render engine side functions */
        void set_scene_object(class SceneObject* object) { m_object = object; }
        void set_name(StringId name) { m_name = std::move(name); }
        void set_visible(bool visible) { m_is_visible = visible; }

        /* render engine side functions */
        const StringId&    get_name() { return m_name; }
        class SceneObject* get_object() { return m_object; }
        class RenderScene* get_render_scene() { return m_render_scene; }
        class GfxDriver*   get_driver() { return m_driver; }
        bool               is_visible() const { return m_is_visible; }

    protected:
        friend class RenderScene;

        class SceneObject* m_object       = nullptr;
        class RenderScene* m_render_scene = nullptr;
        class GfxDriver*   m_driver       = nullptr;
        StringId           m_name         = SID("render-object");
        bool               m_is_visible   = true;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_OBJECT_HPP

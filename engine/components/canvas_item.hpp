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

#ifndef WMOGE_CANVAS_ITEM_HPP
#define WMOGE_CANVAS_ITEM_HPP

#include "render/aux_draw_canvas.hpp"
#include "render/objects/render_canvas_item.hpp"
#include "resource/material.hpp"
#include "scene/scene_component.hpp"

namespace wmoge {

    /**
     * @class CanvasItem
     * @brief Base class for any item which wants to draw in 2d on screen
     *
     * Canvas item is an object which can be rendered on a scene using 2d camera.
     */
    class CanvasItem : public SceneComponent {
    public:
        WG_OBJECT(CanvasItem, SceneComponent);

        void hide();
        void show();
        void set_layer_id(int id);
        void set_tint(const Color4f& tint);

        const Ref<Material>& get_material() const;
        Vec2f                get_pos_global() const;
        const Mat3x3f&       get_matr_global() const;
        const Mat3x3f&       get_matr_global_inv() const;
        const Color4f&       get_tint() const;
        int                  get_layer_id() const;
        bool                 is_visible() const;

        bool on_load_from_yaml(const YamlConstNodeRef& node) override;
        void on_scene_enter() override;
        void on_transform_updated() override;

    protected:
        class RenderScene*      get_render_scene();
        class CallbackQueue*    get_queue();
        class RenderCanvasItem* get_proxy();
        void                    configure_proxy(RenderCanvasItem* proxy);
        void                    release_proxy();

    private:
        Ref<RenderCanvasItem> m_render_proxy;
        Ref<Material>         m_material;
        Mat3x3f               m_matr_global;
        Mat3x3f               m_matr_global_inv;
        Color4f               m_tint       = Color::WHITE4f;
        int                   m_layer_id   = 0;
        bool                  m_is_visible = true;
    };

}// namespace wmoge

#endif//WMOGE_CANVAS_ITEM_HPP

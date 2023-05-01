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

#include "canvas_item.hpp"

#include "components/camera_2d.hpp"
#include "components/canvas_layer.hpp"
#include "components/spatial_2d.hpp"
#include "core/engine.hpp"
#include "render/aux_draw_canvas.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene.hpp"
#include "scene/scene_object.hpp"

namespace wmoge {

    void CanvasItem::hide() {
        assert(get_proxy());

        m_is_visible = false;

        get_queue()->push([proxy = m_render_proxy]() {
            proxy->set_visible(false);
        });
    }
    void CanvasItem::show() {
        assert(get_proxy());

        m_is_visible = true;

        get_queue()->push([proxy = m_render_proxy]() {
            proxy->set_visible(true);
        });
    }
    void CanvasItem::set_layer_id(int id) {
        assert(get_proxy());

        m_layer_id = id;

        get_queue()->push([proxy = m_render_proxy, id]() {
            proxy->set_layer_id(id);
        });
    }
    void CanvasItem::set_tint(const Color4f& color) {
        assert(get_proxy());

        m_tint = color;

        get_queue()->push([proxy = m_render_proxy, color]() {
            proxy->set_tint(color);
        });
    }

    const ref_ptr<Material>& CanvasItem::get_material() const {
        return m_material;
    }
    Vec2f CanvasItem::get_pos_global() const {
        return Math2d::transform(m_matr_global, Vec2f());
    }
    const Mat3x3f& CanvasItem::get_matr_global() const {
        return m_matr_global;
    }
    const Mat3x3f& CanvasItem::get_matr_global_inv() const {
        return m_matr_global_inv;
    }
    const Color4f& CanvasItem::get_tint() const {
        return m_tint;
    }
    int CanvasItem::get_layer_id() const {
        return m_layer_id;
    }
    bool CanvasItem::is_visible() const {
        return m_is_visible;
    }

    bool CanvasItem::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneComponent::on_load_from_yaml(node)) {
            return false;
        }

        if (node.has_child("shader")) {
            ResourceManager* res_manager = Engine::instance()->resource_manager();
            const StringId   shader_name = Yaml::read_sid(node["shader"]);
            const auto       shader      = res_manager->load(shader_name).cast<MaterialShader>();

            if (!shader) {
                WG_LOG_ERROR("failed to load shader " << shader << " for " << get_scene_object()->get_name());
                return false;
            }

            m_material = make_ref<Material>();
            m_material->set_name(get_scene_object()->get_name());
            m_material->create(shader);
        }
        if (node.has_child("tint")) {
            m_tint = Yaml::read_vec4f(node["tint"]);
        }
        if (node.has_child("is_visible")) {
            node["is_visible"] >> m_is_visible;
        }

        return true;
    }
    void CanvasItem::on_scene_enter() {
        SceneComponent::on_scene_enter();

        SceneObject* parent = get_scene_object();
        while (parent) {
            if (CanvasLayer* layer = parent->get<CanvasLayer>()) {
                m_layer_id = layer->get_id();
                break;
            }
            parent = parent->get_parent();
        }

        m_matr_global     = get_scene_object()->get_or_create<Spatial2d>()->get_matr_global();
        m_matr_global_inv = get_scene_object()->get_or_create<Spatial2d>()->get_matr_global_inv();
    }
    void CanvasItem::on_transform_updated() {
        SceneComponent::on_transform_updated();
        m_matr_global     = get_scene_object()->get_or_create<Spatial2d>()->get_matr_global();
        m_matr_global_inv = get_scene_object()->get_or_create<Spatial2d>()->get_matr_global_inv();

        if (get_proxy()) {
            get_queue()->push([proxy = m_render_proxy, new_transform = m_matr_global, new_transform_inv = m_matr_global_inv]() {
                proxy->set_transform(new_transform, new_transform_inv);
            });
        }
    }

    class RenderScene* CanvasItem::get_render_scene() {
        return get_scene()->get_render_scene();
    }
    class CallbackQueue* CanvasItem::get_queue() {
        return get_scene()->get_render_scene()->get_queue();
    }
    class RenderCanvasItem* CanvasItem::get_proxy() {
        return m_render_proxy.get();
    }
    void CanvasItem::configure_proxy(RenderCanvasItem* proxy) {
        assert(proxy);
        assert(!m_render_proxy.get());
        m_render_proxy = ref_ptr<RenderCanvasItem>(proxy);
        m_render_proxy->set_tint(m_tint);
        m_render_proxy->set_visible(m_is_visible);
        m_render_proxy->set_material(m_material);
        m_render_proxy->set_layer_id(m_layer_id);
        m_render_proxy->set_transform(m_matr_global, m_matr_global_inv);
        m_render_proxy->set_scene_object(get_scene_object());
        m_render_proxy->set_name(get_scene_object()->get_name());
    }
    void CanvasItem::release_proxy() {
        assert(m_render_proxy.get());
        m_render_proxy.reset();
    }

}// namespace wmoge
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

#include "sprite_instance.hpp"

#include "components/camera_2d.hpp"
#include "core/engine.hpp"
#include "render/render_engine.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene.hpp"
#include "scene/scene_object.hpp"

namespace wmoge {

    void SpriteInstance::play_animation(const StringId& animation) {
        int id = m_sprite->get_animation_id(animation);

        if (id < 0) {
            WG_LOG_ERROR("failed to find animation " << animation);
            return;
        }

        play_animation(id);
    }
    void SpriteInstance::play_animation(int animation_id) {
        assert(get_proxy());

        m_animation = animation_id;
        m_playing   = true;

        get_queue()->push([proxy = dynamic_cast<RenderSpriteInstance*>(get_proxy()), animation_id]() {
            proxy->set_animation(animation_id);
            proxy->set_playing(true);
        });
    }
    void SpriteInstance::stop_animation() {
        assert(get_proxy());

        m_playing = false;

        get_queue()->push([proxy = dynamic_cast<RenderSpriteInstance*>(get_proxy())]() {
            proxy->set_playing(false);
        });
    }

    const Ref<Sprite>& SpriteInstance::get_sprite() {
        return m_sprite;
    }

    bool SpriteInstance::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!CanvasItem::on_load_from_yaml(node)) {
            return false;
        }

        auto res = Yaml::read_sid(node["sprite"]);
        m_sprite = Engine::instance()->resource_manager()->load(res).cast<Sprite>();

        if (!m_sprite) {
            WG_LOG_ERROR("failed to load sprite " << res);
            return false;
        }

        m_animation = 0;

        if (node.has_child("animation")) {
            m_animation = Yaml::read_int(node["animation"]);
        }
        if (node.has_child("playing")) {
            m_playing = Yaml::read_bool(node["playing"]);
        }

        return true;
    }
    void SpriteInstance::on_scene_enter() {
        CanvasItem::on_scene_enter();

        auto proxy = Engine::instance()->render_engine()->make_sprite_instance();
        configure_proxy(proxy.get());
        proxy->initialize(m_sprite, m_animation, m_speed_scale, m_playing);

        get_queue()->push([scene = get_render_scene(), proxy]() {
            scene->add_object(proxy);
        });
    }
    void SpriteInstance::on_scene_exit() {
        CanvasItem::on_scene_exit();

        get_queue()->push([scene = get_render_scene(), proxy = Ref<RenderCanvasItem>(get_proxy())]() {
            scene->remove_object(proxy);
        });

        release_proxy();
    }

}// namespace wmoge
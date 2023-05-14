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

#include "camera_2d.hpp"

#include "core/engine.hpp"
#include "event/event_manager.hpp"
#include "event/event_window.hpp"
#include "platform/window_manager.hpp"
#include "render/render_engine.hpp"
#include "render/render_scene.hpp"

namespace wmoge {

    void Camera2d::set_background(const Color4f& background) {
        m_background = background;

        get_queue()->push([proxy = dynamic_cast<RenderCamera2d*>(get_proxy()), background]() {
            proxy->set_background(background);
        });
    }
    void Camera2d::set_screen_space(const Vec2f& screen_space) {
        m_screen_space = screen_space;

        get_queue()->push([proxy = dynamic_cast<RenderCamera2d*>(get_proxy()), screen_space]() {
            proxy->set_screen_space(screen_space);
        });
    }
    void Camera2d::set_viewport_rect(const Vec4f& viewport_rect) {
        m_viewport_rect = viewport_rect;

        get_queue()->push([proxy = dynamic_cast<RenderCamera2d*>(get_proxy()), viewport_rect]() {
            proxy->set_viewport_rect(viewport_rect);
        });
    }
    void Camera2d::set_window(const Ref<Window>& window) {
        m_window = window;

        get_queue()->push([proxy = dynamic_cast<RenderCamera2d*>(get_proxy()), window]() {
            proxy->set_window(window);
        });
    }

    const Color4f& Camera2d::get_background() const {
        return m_background;
    }
    const Vec2f& Camera2d::get_screen_space() const {
        return m_screen_space;
    }
    const Vec4f& Camera2d::get_viewport_rect() const {
        return m_viewport_rect;
    }
    const Ref<Window>& Camera2d::get_window() const {
        return m_window;
    }

    bool Camera2d::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneComponent::on_load_from_yaml(node)) {
            return false;
        }

        m_window = Engine::instance()->window_manager()->primary_window();

        if (node.has_child("background")) {
            m_background = Yaml::read_vec4f(node["background"]);
        }
        if (node.has_child("screen_space")) {
            m_screen_space = Yaml::read_vec2f(node["screen_space"]);
        }
        if (node.has_child("viewport_rect")) {
            m_viewport_rect = Yaml::read_vec4f(node["viewport_rect"]);
        }
        if (node.has_child("window")) {
            m_window = Engine::instance()->window_manager()->get(Yaml::read_sid(node["window"]));
        }

        return true;
    }
    void Camera2d::on_scene_enter() {
        CanvasItem::on_scene_enter();

        auto proxy = Engine::instance()->render_engine()->make_camera_2d();
        configure_proxy(proxy.get());
        proxy->set_background(m_background);
        proxy->set_screen_space(m_screen_space);
        proxy->set_viewport_rect(m_viewport_rect);
        proxy->set_window(m_window);

        get_queue()->push([scene = get_render_scene(), proxy]() {
            scene->add_camera(proxy);
        });
    }
    void Camera2d::on_scene_exit() {
        CanvasItem::on_scene_exit();

        get_queue()->push([scene = get_render_scene(), proxy = Ref<RenderCanvasItem>(get_proxy()).cast<RenderCamera2d>()]() {
            scene->remove_camera(proxy);
        });

        release_proxy();
    }

}// namespace wmoge
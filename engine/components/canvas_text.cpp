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

#include "canvas_text.hpp"

#include "core/engine.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "render/aux_draw_canvas.hpp"
#include "render/objects/render_canvas_text.hpp"
#include "render/render_engine.hpp"
#include "render/render_scene.hpp"
#include "resource/resource_manager.hpp"

namespace wmoge {

    void CanvasText::set_text(std::string text) {
        assert(get_proxy());

        m_text = std::move(text);

        get_queue()->push([proxy = dynamic_cast<RenderCanvasText*>(get_proxy()), text = m_text]() {
            proxy->set_text(text);
        });
    }
    void CanvasText::set_font(Ref<Font> font) {
        assert(get_proxy());

        m_font = std::move(font);

        get_queue()->push([proxy = dynamic_cast<RenderCanvasText*>(get_proxy()), font]() {
            proxy->set_font(font);
        });
    }
    void CanvasText::set_font_size(float font_size) {
        assert(get_proxy());

        m_font_size = font_size;

        get_queue()->push([proxy = dynamic_cast<RenderCanvasText*>(get_proxy()), font_size]() {
            proxy->set_font_size(font_size);
        });
    }
    void CanvasText::set_centered() {
        assert(get_proxy());

        m_alignment = FontTextAlignment::Center;

        get_queue()->push([proxy = dynamic_cast<RenderCanvasText*>(get_proxy())]() {
            proxy->set_alignment(FontTextAlignment::Center);
        });
    }
    const std::string& CanvasText::get_text() {
        return m_text;
    }
    const Ref<Font>& CanvasText::get_font() {
        return m_font;
    }
    float CanvasText::get_font_size() {
        return m_font_size;
    }

    bool CanvasText::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!CanvasItem::on_load_from_yaml(node)) {
            return false;
        }

        m_text      = Yaml::read_str(node["text"]);
        m_font_size = Yaml::read_float(node["font_size"]);

        auto font_res = Yaml::read_sid(node["font"]);
        m_font        = Engine::instance()->resource_manager()->load(font_res).cast<Font>();

        if (!m_font) {
            WG_LOG_ERROR("failed to load font " << font_res);
            return false;
        }

        if (node.has_child("alignment")) {
            m_alignment = Enum::parse<FontTextAlignment>(node["alignment"]);
        }

        return true;
    }
    void CanvasText::on_scene_enter() {
        CanvasItem::on_scene_enter();

        auto proxy = Engine::instance()->render_engine()->make_canvas_text();
        configure_proxy(proxy.get());
        proxy->initialize(m_text, m_font, m_font_size, m_alignment);

        get_queue()->push([scene = get_render_scene(), proxy]() {
            scene->add_object(proxy);
        });
    }
    void CanvasText::on_scene_exit() {
        CanvasItem::on_scene_exit();

        get_queue()->push([scene = get_render_scene(), proxy = Ref<RenderCanvasItem>(get_proxy())]() {
            scene->remove_object(proxy);
        });

        release_proxy();
    }

}// namespace wmoge

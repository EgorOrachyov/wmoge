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

#include "particles_2d.hpp"

#include "core/engine.hpp"
#include "pfx/pfx_scene.hpp"
#include "render/render_engine.hpp"
#include "render/render_scene.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene.hpp"

namespace wmoge {

    void Particles2d::emit(int amount) {
        PfxSpawnParams params;
        params.amount = amount;
        params.pos2d  = get_pos_global();
        emit(params);
    }
    void Particles2d::emit(const PfxSpawnParams& params) {
        assert(m_emitter);
        assert(params.amount > 0);

        m_emitter->emit(params);
    }

    bool Particles2d::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!CanvasItem::on_load_from_yaml(node)) {
            return false;
        }

        auto res    = Yaml::read_sid(node["effect"]);
        auto effect = Engine::instance()->resource_manager()->load(res).cast<PfxEffect>();

        if (!effect) {
            WG_LOG_ERROR("failed to load effect " << res);
            return false;
        }

        m_effect = effect;

        return true;
    }

    void Particles2d::on_scene_enter() {
        CanvasItem::on_scene_enter();

        auto proxy = Engine::instance()->render_engine()->make_particles_2d();
        configure_proxy(proxy.get());

        m_emitter = make_ref<PfxEmitter>(m_effect, get_render_scene());
        proxy->initialize(m_emitter);

        get_queue()->push([scene = get_render_scene(), proxy]() {
            scene->add_object(proxy);
        });

        get_scene()->get_pfx_scene()->add_emitter(m_emitter);
    }
    void Particles2d::on_scene_exit() {
        CanvasItem::on_scene_exit();

        get_scene()->get_pfx_scene()->remove_emitter(m_emitter);

        get_queue()->push([scene = get_render_scene(), proxy = ref_ptr<RenderCanvasItem>(get_proxy())]() {
            scene->remove_object(proxy);
        });

        release_proxy();
        m_emitter.reset();
    }

}// namespace wmoge

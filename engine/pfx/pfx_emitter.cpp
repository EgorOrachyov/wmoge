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

#include "pfx_emitter.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "resource/pfx_effect.hpp"

namespace wmoge {

    PfxEmitter::PfxEmitter(ref_ptr<class PfxEffect> effect, class RenderScene* scene) {
        assert(effect);
        assert(scene);

        m_effect = std::move(effect);
        m_scene  = scene;

        int components_count = m_effect->get_components_count();
        assert(components_count);

        for (int id = 0; id < components_count; id += 1) {
            auto* component = m_effect->get_component(id).get();
            m_runtime.emplace_back(this, component);
        }
    }

    void PfxEmitter::emit(const PfxSpawnParams& params) {
        std::lock_guard guard(m_mutex);
        m_emit_queue.push_back(params);
    }
    void PfxEmitter::advance_time(float delta_time) {
        m_delta_time = delta_time;
        m_time += delta_time;
    }
    void PfxEmitter::update() {
        WG_AUTO_PROFILE_PFX();

        fast_vector<PfxSpawnParams> emit_queue;
        {
            std::lock_guard guard(m_mutex);
            std::swap(emit_queue, m_emit_queue);
        }

        for (const auto& params : emit_queue) {
            for (auto& runtime : m_runtime) {
                runtime.emit(params);
            }
        }
        for (auto& runtime : m_runtime) {
            runtime.update(m_delta_time);
        }
    }
    void PfxEmitter::render(RenderViewList& views, const RenderViewMask& mask, RenderObject* object) {
        WG_AUTO_PROFILE_PFX();

        for (auto& runtime : m_runtime) {
            runtime.render(views, mask, object);
        }
    }
    const PfxComponentRuntime& PfxEmitter::get_runtime(int id) const {
        assert(id < m_runtime.size());
        return m_runtime[id];
    }
    const ref_ptr<PfxComponent>& PfxEmitter::get_component(int id) const {
        assert(m_effect);
        return m_effect->get_component(id);
    }
    const ref_ptr<PfxEffect>& PfxEmitter::get_effect() const {
        return m_effect;
    }
    class RenderScene* PfxEmitter::get_scene() const {
        return m_scene;
    }
    float PfxEmitter::get_time() const {
        return m_time;
    }
    float PfxEmitter::get_delta_time() const {
        return m_delta_time;
    }
    int PfxEmitter::get_num_components() const {
        return int(m_runtime.size());
    }
    bool PfxEmitter::is_active() const {
        return m_is_active;
    }

}// namespace wmoge

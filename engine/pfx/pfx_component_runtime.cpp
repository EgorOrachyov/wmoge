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

#include "pfx_component_runtime.hpp"

#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "pfx/pfx_emitter.hpp"

#include <cassert>

namespace wmoge {

    PfxComponentRuntime::PfxComponentRuntime(class PfxEmitter* emitter, class PfxComponent* component) {
        assert(emitter);
        assert(component);

        m_emitter   = emitter;
        m_component = component;

        auto attributes = component->get_attributes();
        assert(attributes.bits.any());

        m_storage = std::make_unique<PfxStorage>(attributes, component->get_amount());

        for (int i = 0; i < m_component->get_features_count(); i++) {
            m_component->get_feature(i)->on_provided_render(*this, m_renderer);
        }
    }

    void PfxComponentRuntime::emit(const PfxSpawnParams& params) {
        WG_AUTO_PROFILE_PFX();

        const auto& attributes = m_storage->get_attributes();

        m_spawned_from   = (m_active_from + m_active_amount) % m_storage->get_capacity();
        m_spawned_amount = params.amount;

        auto view_p   = m_storage->get_pos2d();
        auto view_v   = m_storage->get_vel2d();
        auto view_a   = m_storage->get_angle();
        auto view_c   = m_storage->get_color();
        auto view_s   = m_storage->get_size();
        auto view_t   = m_storage->get_time();
        auto view_tn  = m_storage->get_time_norm();
        auto view_ttl = m_storage->get_time_to_live();

        for (auto particle_id : get_spawn_range()) {
            if (attributes.get(PfxAttribute::Pos2d)) {
                view_p[particle_id] = params.pos2d;
            }
            if (attributes.get(PfxAttribute::Vel2d)) {
                view_v[particle_id] = params.vel2d;
            }
            if (attributes.get(PfxAttribute::Angle)) {
                view_a[particle_id] = params.angle;
            }
            if (attributes.get(PfxAttribute::Color)) {
                view_c[particle_id] = params.color;
            }
            if (attributes.get(PfxAttribute::Size)) {
                view_s[particle_id] = params.size;
            }
            if (attributes.get(PfxAttribute::Time)) {
                view_t[particle_id] = 0.0f;
            }
            if (attributes.get(PfxAttribute::TimeNorm)) {
                view_tn[particle_id] = 0.0f;
            }
            if (attributes.get(PfxAttribute::TimeToLive)) {
                view_ttl[particle_id] = params.lifetime;
            }
        }

        for (int i = 0; i < m_component->get_features_count(); i++) {
            m_component->get_feature(i)->on_spawn(*this, params);
        }

        if (m_active_amount + m_spawned_amount <= m_storage->get_capacity()) {
            m_active_amount += m_spawned_amount;
        } else {
            m_active_amount = m_storage->get_capacity();
            m_active_from   = (m_spawned_from + m_spawned_amount) % m_storage->get_capacity();
        }

        m_spawned_from   = -1;
        m_spawned_amount = -1;
        m_is_active      = m_active_amount > 0;
    }
    void PfxComponentRuntime::update(float dt) {
        WG_AUTO_PROFILE_PFX();

        for (int i = 0; i < m_component->get_features_count(); i++) {
            m_component->get_feature(i)->on_update(*this, dt);
        }
        for (int i = 0; i < m_component->get_features_count(); i++) {
            m_component->get_feature(i)->on_prepare_render(*this);
        }

        if (m_storage->get_attributes().get(PfxAttribute::TimeNorm)) {
            auto view_tn = m_storage->get_time_norm();

            for (auto particle_id : get_update_range()) {
                if (view_tn[particle_id] >= 1.0f) {
                    m_active_amount -= 1;
                    m_active_from = (m_active_from + 1) % m_storage->get_capacity();
                }
            }

            assert(m_active_amount >= 0);
            m_is_active = m_active_amount > 0;
        }
    }
    void PfxComponentRuntime::render(RenderViewList& views, const RenderViewMask& mask, RenderObject* object) {
        WG_AUTO_PROFILE_PFX();

        if (m_renderer.get()) {
            m_renderer->on_render_dynamic(views, mask, object);
        }
    }

    PfxRange PfxComponentRuntime::get_update_range() {
        return PfxRange(m_active_from, m_active_amount, m_storage->get_capacity());
    }
    PfxRange PfxComponentRuntime::get_spawn_range() {
        return PfxRange(m_spawned_from, m_spawned_amount, m_storage->get_capacity());
    }
    PfxRange PfxComponentRuntime::get_full_range() {
        return PfxRange(0, m_storage->get_capacity(), m_storage->get_capacity());
    }

    PfxStorage* PfxComponentRuntime::get_storage() {
        return m_storage.get();
    }
    PfxRenderer* PfxComponentRuntime::get_renderer() {
        return m_renderer.get();
    }
    Aabbf& PfxComponentRuntime::get_bounds() {
        return m_bounds;
    }
    class PfxEmitter* PfxComponentRuntime::get_emitter() {
        return m_emitter;
    }
    class PfxComponent* PfxComponentRuntime::get_component() {
        return m_component;
    }
    bool PfxComponentRuntime::get_is_active() const {
        return m_is_active;
    }
    int PfxComponentRuntime::get_active_amount() const {
        return m_active_amount;
    }
    int PfxComponentRuntime::get_spawned_amount() const {
        return m_spawned_amount;
    }

}// namespace wmoge
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

#include "pfx_feature_sprite.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "math/color.hpp"
#include "math/math_utils2d.hpp"
#include "math/transform.hpp"
#include "pfx/pfx_component_runtime.hpp"
#include "pfx/pfx_emitter.hpp"
#include "pfx/renderers/pfx_sprite_renderer.hpp"
#include "render/aux_draw_canvas.hpp"
#include "render/render_scene.hpp"
#include "resource/resource_manager.hpp"
#include "resource/sprite.hpp"

namespace wmoge {

    Ref<PfxFeature> PfxFeatureSprite::create() const {
        return make_ref<PfxFeatureSprite>();
    }
    StringId PfxFeatureSprite::get_feature_name() const {
        static StringId name("Sprite");
        return name;
    }
    StringId PfxFeatureSprite::get_feature_family() const {
        static StringId family("Visual");
        return family;
    }
    bool PfxFeatureSprite::load_from_options(const YamlConstNodeRef& node) {
        StringId sprite;
        StringId shader;

        Yaml::read(node["sprite"], sprite);
        if (sprite.empty()) {
            WG_LOG_ERROR("empty sprite name for feature");
            return false;
        }

        Yaml::read(node["shader"], shader);
        if (shader.empty()) {
            WG_LOG_ERROR("empty shader name for feature");
            return false;
        }

        auto* resource_manager = Engine::instance()->resource_manager();

        m_sprite = resource_manager->load(sprite).cast<Sprite>();
        if (!m_sprite) {
            WG_LOG_ERROR("failed to load sprite " << sprite);
            return false;
        }

        m_shader = resource_manager->load(shader).cast<Shader>();
        if (!m_shader) {
            WG_LOG_ERROR("failed to load shader " << shader);
            return false;
        }

        node["animation"] >> m_animation;

        return true;
    }
    void PfxFeatureSprite::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::Time);
        attributes.set(PfxAttribute::Pos2d);
        attributes.set(PfxAttribute::Size);
        attributes.set(PfxAttribute::Angle);
        attributes.set(PfxAttribute::Color);
    }
    void PfxFeatureSprite::on_provided_render(class PfxComponentRuntime& runtime, std::unique_ptr<class PfxRenderer>& renderer) {
        WG_AUTO_PROFILE_PFX("PfxFeatureSprite::on_provided_render");

        Ref<Material> material = make_ref<Material>();
        material->create(m_shader.as<Shader>());

        if (m_animation != -1) {
            Ref<Texture2d> texture = m_sprite->get_animation_texture(m_animation);
            material->set_texture(SID("sprite"), texture);
        }

        int n_vertices = runtime.get_component()->get_amount() * sizeof(GfxVF_Pos2Uv2Col4[N_VERTICES_PER_PARTICLE]);
        int n_indices  = runtime.get_component()->get_amount() * sizeof(std::uint16_t[N_INDICES_PER_PARTICLE]);

        renderer = std::make_unique<PfxSpriteRenderer>(material, n_vertices, n_indices, runtime.get_component()->get_name());
    }
    void PfxFeatureSprite::on_prepare_render(PfxComponentRuntime& runtime) {
        WG_AUTO_PROFILE_PFX("PfxFeatureSprite::on_prepare_render");

        auto  emitter  = Ref<PfxEmitter>(runtime.get_emitter());
        auto* renderer = runtime.get_renderer();

        if ((m_animation == -1) || (runtime.get_active_amount() == 0)) {
            runtime.get_emitter()->get_scene()->get_queue()->push([emitter, renderer]() {
                renderer->on_update_data(PfxComponentRenderData{});
            });

            return;
        }

        const int n_active_particles = runtime.get_active_amount();

        PfxComponentRenderData render_data;
        render_data.vertices = make_ref<Data>(sizeof(GfxVF_Pos2Uv2Col4[N_VERTICES_PER_PARTICLE]) * n_active_particles);
        render_data.indices  = make_ref<Data>(sizeof(std::uint16_t[N_INDICES_PER_PARTICLE]) * n_active_particles);

        GfxVF_Pos2Uv2Col4* vertices = reinterpret_cast<GfxVF_Pos2Uv2Col4*>(render_data.vertices->buffer());
        std::uint16_t*     indices  = reinterpret_cast<std::uint16_t*>(render_data.indices->buffer());

        auto* storage = runtime.get_storage();
        auto  view_t  = storage->get_time();
        auto  view_p  = storage->get_pos2d();
        auto  view_a  = storage->get_angle();
        auto  view_s  = storage->get_size();
        auto  view_c  = storage->get_color();

        const auto& texture       = m_sprite->get_animation_texture(m_animation);
        const float anim_speed    = m_sprite->get_animation_speed(m_animation);
        const int   anim_n_frames = m_sprite->get_animation_n_frames(m_animation);
        const bool  anim_loop     = m_sprite->get_animation_loop(m_animation);
        const auto  size          = m_sprite->get_size();
        const auto  pivot         = m_sprite->get_pivot();

        for (auto particle_id : runtime.get_update_range()) {
            const float t           = view_t[particle_id];
            const float frame_accum = t * anim_speed;
            const int   curr_frame  = int(Math::floor(frame_accum));
            const int   frame       = anim_loop ? curr_frame % anim_n_frames : Math::min(curr_frame, anim_n_frames - 1);

            assert(0 < anim_n_frames);
            assert(0 <= frame && frame < anim_n_frames);

            const auto& frame_uv       = m_sprite->get_animation_frame(m_animation, frame);
            const float particle_scale = view_s[particle_id];
            const float particle_angle = view_a[particle_id];
            const auto  particle_pos   = view_p[particle_id];
            const auto  particle_col   = view_c[particle_id];

            Transform2d transform_2d;
            transform_2d.translate(particle_pos);
            transform_2d.scale(Vec2f(particle_scale, particle_scale));
            transform_2d.rotate(particle_angle);

            const auto mat = transform_2d.get_transform();

            auto& p0 = vertices[render_data.n_vertices + 0];
            auto& p1 = vertices[render_data.n_vertices + 1];
            auto& p2 = vertices[render_data.n_vertices + 2];
            auto& p3 = vertices[render_data.n_vertices + 3];

            p0.pos = Math2d::transform(mat, Vec2f(-pivot.x(), -pivot.y() + size.y()));
            p1.pos = Math2d::transform(mat, Vec2f(-pivot.x(), -pivot.y()));
            p2.pos = Math2d::transform(mat, Vec2f(-pivot.x() + size.x(), -pivot.y()));
            p3.pos = Math2d::transform(mat, Vec2f(-pivot.x() + size.x(), -pivot.y() + size.y()));

            p0.uv = Vec2f(frame_uv.x(), frame_uv.y() + frame_uv.w());
            p1.uv = Vec2f(frame_uv.x(), frame_uv.y());
            p2.uv = Vec2f(frame_uv.x() + frame_uv.z(), frame_uv.y());
            p3.uv = Vec2f(frame_uv.x() + frame_uv.z(), frame_uv.y() + frame_uv.w());

            p0.col = particle_col;
            p1.col = particle_col;
            p2.col = particle_col;
            p3.col = particle_col;

            indices[render_data.n_indices++] = render_data.n_vertices + 0;
            indices[render_data.n_indices++] = render_data.n_vertices + 1;
            indices[render_data.n_indices++] = render_data.n_vertices + 2;
            indices[render_data.n_indices++] = render_data.n_vertices + 2;
            indices[render_data.n_indices++] = render_data.n_vertices + 3;
            indices[render_data.n_indices++] = render_data.n_vertices + 0;

            render_data.n_vertices += N_VERTICES_PER_PARTICLE;
        }

        runtime.get_emitter()->get_scene()->get_queue()->push([emitter, renderer, render_data]() {
            renderer->on_update_data(render_data);
        });
    }

    void PfxFeatureSprite::register_class() {
        auto* cls = Class::register_class<PfxFeatureSprite>();
    }

}// namespace wmoge
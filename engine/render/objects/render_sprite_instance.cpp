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

#include "render_sprite_instance.hpp"

#include "debug/profiler.hpp"
#include "resource/shader_2d.hpp"

namespace wmoge {

    void RenderSpriteInstance::initialize(Ref<Sprite> sprite, int animation, float speed_scale, bool playing) {
        m_sprite      = std::move(sprite);
        m_speed_scale = speed_scale;
        m_playing     = playing;
        set_animation(animation);
    }
    void RenderSpriteInstance::set_animation(int animation) {
        m_animation   = animation;
        m_frame       = 0;
        m_frame_accum = 0.0f;

        if (m_animation != -1) {
            const auto& texture = m_sprite->get_animation_texture(m_animation);
            m_material->set_texture(SID("sprite"), texture);
        }
    }
    void RenderSpriteInstance::set_playing(bool playing) {
        m_playing = playing;
    }

    void RenderSpriteInstance::on_scene_enter() {
        WG_AUTO_PROFILE_RENDER("RenderSpriteInstance::on_scene_enter");

        m_vert_buffer  = m_driver->make_vert_buffer(sizeof(GfxVF_Pos2Uv2Col4[N_VERTICES]), GfxMemUsage::GpuLocal, m_name);
        m_index_buffer = m_driver->make_index_buffer(sizeof(std::uint16_t[N_INDICES]), GfxMemUsage::GpuLocal, m_name);

        create_draw_params();
        create_vert_format();
        compile_draw_cmds(N_INDICES);

        auto  indices_data = make_ref<Data>(sizeof(std::uint16_t[N_INDICES]));
        auto* indices      = reinterpret_cast<std::uint16_t*>(indices_data->buffer());
        indices[0]         = 0;
        indices[1]         = 1;
        indices[2]         = 2;
        indices[3]         = 2;
        indices[4]         = 3;
        indices[5]         = 0;
        m_driver->update_index_buffer(m_index_buffer, 0, m_index_buffer->size(), indices_data);
    }

    void RenderSpriteInstance::on_update(float dt) {
        WG_AUTO_PROFILE_RENDER("RenderSpriteInstance::on_update");

        RenderObject::on_update(dt);

        if (!m_playing || m_animation == -1) return;

        const float anim_speed    = m_sprite->get_animation_speed(m_animation);
        const int   anim_n_frames = m_sprite->get_animation_n_frames(m_animation);
        const bool  anim_loop     = m_sprite->get_animation_loop(m_animation);

        if (m_frame_accum < anim_n_frames) {
            m_frame_accum += m_speed_scale * anim_speed * dt;
        }

        const float curr_frame = Math::floor(m_frame_accum);

        int prev_frame = m_frame;

        if (!anim_loop) {
            m_frame = Math::min(int(curr_frame), anim_n_frames - 1);
        } else {
            m_frame       = int(curr_frame) % anim_n_frames;
            m_frame_accum = m_frame_accum - curr_frame + m_frame;
        }

        if (prev_frame != m_frame) mark_dirty_geom();

        assert(0 < anim_n_frames);
        assert(0 <= m_frame && m_frame < anim_n_frames);
        assert(0 <= m_frame_accum);
    }

    void RenderSpriteInstance::on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) {
        WG_AUTO_PROFILE_RENDER("RenderSpriteInstance::on_render_dynamic");

        if (m_animation == -1) return;

        RenderCanvasItem::on_render_dynamic(views, mask);

        if (m_dirty_geom_data) {
            // check if we need rebuild, since it is a costly operation
            // (requires rebuild of vertex buffer with tmp allocations)
            rebuild_geom();
            m_dirty_geom_data = false;
        }

        for (int view_idx = 0; view_idx < int(views.size()); view_idx++) {
            RenderView* view = views[view_idx];

            if (!mask[view_idx]) {
                continue;
            }
            if (!view->is_overlay_view()) {
                continue;
            }

            for (int cmd_idx = 0; cmd_idx < int(m_cached_cmds.size()); cmd_idx++) {
                DrawCmdSortingKey key = m_cached_keys[cmd_idx];
                DrawCmd*          cmd = m_cached_cmds[cmd_idx];
                view->add_cmd(key, cmd, DrawPass::Overlay2dPass);
            }
        }
    }

    bool RenderSpriteInstance::need_update() const {
        return true;
    }
    bool RenderSpriteInstance::need_render_dynamic() const {
        return true;
    }

    void RenderSpriteInstance::rebuild_geom() {
        WG_AUTO_PROFILE_RENDER("RenderSpriteInstance::rebuild_geom");

        const auto frame   = m_sprite->get_animation_frame(m_animation, m_frame);
        const auto size    = m_sprite->get_size();
        const auto pivot   = m_sprite->get_pivot();
        const auto uv_base = Vec2f(frame.x(), frame.y());
        const auto uv_size = Vec2f(frame.z(), frame.w());

        auto* vertices = reinterpret_cast<GfxVF_Pos2Uv2Col4*>(m_driver->map_vert_buffer(m_vert_buffer));

        vertices[0].pos = Vec2f(-pivot.x(), -pivot.y() + size.y());
        vertices[1].pos = Vec2f(-pivot.x(), -pivot.y());
        vertices[2].pos = Vec2f(-pivot.x() + size.x(), -pivot.y());
        vertices[3].pos = Vec2f(-pivot.x() + size.x(), -pivot.y() + size.y());

        vertices[0].uv = Vec2f(uv_base.x(), uv_base.y() + uv_size.y());
        vertices[1].uv = Vec2f(uv_base.x(), uv_base.y());
        vertices[2].uv = Vec2f(uv_base.x() + uv_size.x(), uv_base.y());
        vertices[3].uv = Vec2f(uv_base.x() + uv_size.x(), uv_base.y() + uv_size.y());

        vertices[0].col = Color::WHITE4f;
        vertices[1].col = Color::WHITE4f;
        vertices[2].col = Color::WHITE4f;
        vertices[3].col = Color::WHITE4f;

        m_driver->unmap_vert_buffer(m_vert_buffer);
    }
    void RenderSpriteInstance::mark_dirty_geom() {
        m_dirty_geom_data = true;
    }

}// namespace wmoge
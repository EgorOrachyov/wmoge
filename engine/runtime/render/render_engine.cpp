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

#include "render_engine.hpp"

#include "../../shaders/tmp/generated/auto_material_reflection.hpp"
#include "core/log.hpp"
#include "core/task_parallel_for.hpp"
#include "gfx/gfx_driver.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

#include <cassert>

namespace wmoge {

    RenderEngine::RenderEngine() {
        WG_LOG_INFO("init render engine");

        m_fullscreen_tria.resize(3);
        m_fullscreen_tria[0].pos = Vec2f(-1, -1);
        m_fullscreen_tria[0].uv  = Vec2f(0, 0);
        m_fullscreen_tria[1].pos = Vec2f(-1, 3);
        m_fullscreen_tria[1].uv  = Vec2f(0, 2);
        m_fullscreen_tria[2].pos = Vec2f(3, -1);
        m_fullscreen_tria[2].uv  = Vec2f(2, 0);
        //  m_fullscreen_tria.flush();
    }

    void RenderEngine::set_time(float time) {
        assert(time >= 0);
        m_time = time;
    }
    void RenderEngine::set_delta_time(float delta_time) {
        assert(delta_time >= 0);
        m_delta_time = delta_time;
    }
    void RenderEngine::set_scene(RenderScene* scene) {
        m_scene = scene;
    }

    void RenderEngine::begin_rendering() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::begin_rendering");

        m_batch_collector.clear();
        m_batch_compiler.clear();
        m_cmd_merger.clear();
        m_cmd_allocator.clear();
        m_queues.clear();
    }

    void RenderEngine::end_rendering() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::end_rendering");
    }

    void RenderEngine::prepare_frame_data() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::prepare_frame_data");

        GfxDriver* gfx_driver = Engine::instance()->gfx_driver();
        // GfxCtx*    gfx_ctx    = Engine::instance()->gfx_ctx();

        // if (!m_frame_data) {
        //     m_frame_data = gfx_driver->make_uniform_buffer(int(sizeof(GPUFrameData)), GfxMemUsage::GpuLocal, SID("frame_data"));
        // }

        // GPUFrameData& frame_data = *((GPUFrameData*) gfx_ctx->map_uniform_buffer(m_frame_data));
        // {
        //     frame_data.time      = m_time;
        //     frame_data.timeDelta = m_delta_time;
        //     frame_data._fd_pad0  = 0.0f;
        //     frame_data._fd_pad1  = 0.0f;
        // }
        // gfx_ctx->unmap_uniform_buffer(m_frame_data);
    }

    void RenderEngine::allocate_veiws() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::allocate_veiws");

        GfxDriver* gfx_driver = Engine::instance()->gfx_driver();
        // GfxCtx*    gfx_ctx    = Engine::instance()->gfx_ctx();
        // Mat4x4f    gfx_clip   = gfx_driver->clip_matrix();

        // for (int view_idx = 0; view_idx < int(m_cameras.get_size()); view_idx++) {
        //     RenderView& view = m_views[view_idx];
        //     view.index       = view_idx;

        //     const CameraData& camera = m_cameras.data_at(view_idx);

        //     if (!view.view_data) {
        //         view.view_data = gfx_driver->make_uniform_buffer(int(sizeof(GPUViewData)), GfxMemUsage::GpuLocal, SID("view_data_" + StringUtils::from_int(view_idx)));
        //     }

        //     GPUViewData& view_data = *((GPUViewData*) gfx_ctx->map_uniform_buffer(view.view_data));
        //     {
        //         view_data.Clip             = gfx_clip.transpose();
        //         view_data.Proj             = camera.proj.transpose();
        //         view_data.View             = camera.view.transpose();
        //         view_data.ProjView         = camera.proj_view.transpose();
        //         view_data.ClipProjView     = (gfx_clip * camera.proj_view).transpose();
        //         view_data.ProjPrev         = camera.proj_prev.transpose();
        //         view_data.ViewPrev         = camera.view_prev.transpose();
        //         view_data.ProjViewPrev     = camera.proj_view_prev.transpose();
        //         view_data.ClipProjViewPrev = (gfx_clip * camera.proj_view_prev).transpose();
        //         view_data.Movement         = Vec4f(camera.movement, 0.0f);
        //         view_data.Position         = Vec4f(camera.position, 0.0f);
        //         view_data.Direction        = Vec4f(camera.direction, 0.0f);
        //         view_data.Up               = Vec4f(camera.up, 0.0f);
        //         view_data.PositionPrev     = Vec4f(camera.position_prev, 0.0f);
        //         view_data.DirectionPrev    = Vec4f(camera.direction_prev, 0.0f);
        //         view_data.UpPrev           = Vec4f(camera.up_prev, 0.0f);
        //         view_data.Viewport         = camera.viewport;
        //         view_data.CamIdx           = view_idx;
        //         view_data._vd_pad0         = 0;
        //         view_data._vd_pad1         = 0;
        //         view_data._vd_pad2         = 0;
        //     }
        //     gfx_ctx->unmap_uniform_buffer(view.view_data);

        //     GfxDescSetResources view_resources;
        //     {
        //         {
        //             auto& r               = view_resources.emplace_back();
        //             r.first.type          = GfxBindingType::UniformBuffer;
        //             r.first.binding       = ShaderMaterial::FRAMEDATA_SLOT;
        //             r.first.array_element = 0;
        //             r.second.resource     = m_frame_data.as<GfxResource>();
        //             r.second.offset       = 0;
        //             r.second.range        = m_frame_data->size();
        //         }
        //         {
        //             auto& r               = view_resources.emplace_back();
        //             r.first.type          = GfxBindingType::UniformBuffer;
        //             r.first.binding       = ShaderMaterial::VIEWDATA_SLOT;
        //             r.first.array_element = 0;
        //             r.second.resource     = view.view_data.as<GfxResource>();
        //             r.second.offset       = 0;
        //             r.second.range        = view.view_data->size();
        //         }
        //         {
        //             auto& r               = view_resources.emplace_back();
        //             r.first.type          = GfxBindingType::StorageBuffer;
        //             r.first.binding       = ShaderMaterial::RENDEROBJECTSDATA_SLOT;
        //             r.first.array_element = 0;
        //             r.second.resource     = m_scene->get_objects_gpu_data().get_buffer().as<GfxResource>();
        //             r.second.offset       = 0;
        //             r.second.range        = m_scene->get_objects_gpu_data().get_buffer()->size();
        //         }
        //     }
        //     // view.view_set = gfx_driver->make_desc_set(view_resources, SID("view_set_" + StringUtils::from_int(view_idx)));

        //     for (RenderQueue& queue : view.queues) {
        //         queue.clear();
        //     }
        // }
    }

    void RenderEngine::compile_batches() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::compile_batches");

        const array_view<const MeshBatch> batches = m_batch_collector.get_batches();

        m_batch_compiler.set_scene(m_scene);
        m_batch_compiler.set_cameras(m_cameras);
        m_batch_compiler.set_views(get_views());
        m_batch_compiler.set_cmd_allocator(m_cmd_allocator);

        TaskParallelFor task_compile(SID("compile_batches"), [&](TaskContext&, int id, int) {
            m_batch_compiler.compile_batch(batches[id], id);
            return 0;
        });

        task_compile.schedule(int(batches.size()), m_batch_size).wait_completed();
    }

    void RenderEngine::group_queues() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::group_queues");

        std::size_t total_sorted_cmds = 0;

        for (RenderView& view : get_views()) {
            for (int i = 0; i < RenderView::QUEUE_COUNT; i++) {
                if (!view.queues[i].is_empty()) {
                    total_sorted_cmds += view.queues[i].get_size();
                    m_queues.push_back(&view.queues[i]);
                }
            }
        }

        m_scene->get_objects_ids().clear();
        m_scene->get_objects_ids().resize(total_sorted_cmds);
    }

    void RenderEngine::sort_queues() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::sort_queues");

        TaskParallelFor task_sort(SID("sort_queues"), [&](TaskContext&, int id, int) {
            m_queues[id]->sort();
            return 0;
        });

        task_sort.schedule(int(m_queues.size()), 1).wait_completed();
    }

    void RenderEngine::merge_cmds() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::merge_cmds");

        m_cmd_merger.set_scene(m_scene);
        m_cmd_merger.set_cmd_allocator(m_cmd_allocator);

        TaskParallelFor task_merge(SID("merge_cmds"), [&](TaskContext&, int id, int) {
            m_cmd_merger.proccess_queue(*m_queues[id]);
            return 0;
        });

        task_merge.schedule(int(m_queues.size()), 1).wait_completed();
    }

    void RenderEngine::flush_buffers() {
        WG_AUTO_PROFILE_RENDER("RenderEngine::flush_buffers");

        // m_scene->flush_buffers(Engine::instance()->gfx_ctx());
    }

    void RenderEngine::render_canvas(Canvas& canvas, const Vec4f& area) {
        WG_AUTO_PROFILE_RENDER("RenderEngine::render_canvas");

        if (m_cameras.is_empty()) {
            return;
        }

        const CameraData& main_cam = m_cameras.camera_main();
        // canvas.render(m_main_target, main_cam.viewport, area, 2.2f);
    }

    void RenderEngine::render_aux_geom(AuxDrawManager& aux_draw_manager) {
        WG_AUTO_PROFILE_RENDER("RenderEngine::render_aux_geom");

        if (m_cameras.is_empty()) {
            return;
        }

        const CameraData& main_cam = m_cameras.camera_main();
        // aux_draw_manager.render(m_main_target, main_cam.viewport, main_cam.proj_view);
    }

}// namespace wmoge
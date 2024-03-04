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

#pragma once

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_vector.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/color.hpp"
#include "mesh/mesh_batch.hpp"
#include "mesh/mesh_pass.hpp"
#include "platform/window.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/camera.hpp"
#include "render/canvas.hpp"
#include "render/render_defs.hpp"
#include "render/render_queue.hpp"
#include "render/render_scene.hpp"

#include <array>
#include <optional>

namespace wmoge {

    /**
     * @class RenderSettings
     * @brief Global rendering settings for the engine
    */
    struct RenderSettings {
        float gamma = 2.2f;
    };

    /**
     * @class RenderView
     * @brief Holds data required to render a single view
    */
    struct RenderView {
        static constexpr int QUEUE_COUNT = MESH_PASSES_TOTAL;

        std::array<RenderQueue, QUEUE_COUNT> queues;
        Ref<GfxUniformBuffer>                view_data;
        Ref<GfxDescSet>                      view_set;
        int                                  index = 0;
    };

    /**
     * @class LodValue
     * @brief Selector to draw model lods with otional transition
    */
    struct LodValue {
        int                  current_lod = 0;
        std::optional<int>   next_lod;
        std::optional<float> transition;
    };

    /**
     * @class RenderParams
     * @brief Params passed to draw a particular object
    */
    struct RenderParams {
        LodValue                                      lod_value;
        RenderCameraMask                              camera_mask;
        fast_vector<float, RenderLimits::MAX_CAMERAS> camera_dists;
    };

    /**
     * @class RenderPassInfo
     * @brief Global render state passed to objects during rendering
    */
    struct RenderPassInfo {
        ArrayView<RenderView> views;
        CameraList*           cameras   = nullptr;
        MeshBatchCollector*   collector = nullptr;
    };

    /**
     * @class RenderEngine
     * @brief Global rendering engine responsible for visualization of render objects
     * 
     * Render engine is a global manager responsiple for rendering of objects for each frame.
     * It is used by a scene manager to render an active scene to a screen.
     * 
     * Render engine itself is lower-level class, which operates render objects,
     * list of cameras, params and etc. to collect requrest of batch elements from 
     * different objects (which wants to be rendered), compile it into optimized
     * render commands and submit commands to the GPU using selected rendering path.
     * 
     * @see RenderObject
     * @see RenderCamera
     * @see RenderCameras
     * @see RenderObjectCollector
     * @see MeshBatchCollector
     * @see MeshBatchCompiler
    */
    class RenderEngine {
    public:
        RenderEngine();

        void set_time(float time);
        void set_delta_time(float delta_time);
        void set_scene(RenderScene* scene);

        void begin_rendering();
        void end_rendering();

        void prepare_frame_data();
        void allocate_veiws();
        void compile_batches();
        void group_queues();
        void sort_queues();
        void merge_cmds();
        void flush_buffers();
        void render_canvas(Canvas& canvas, const Vec4f& area);
        void render_aux_geom(AuxDrawManager& aux_draw_manager);

        [[nodiscard]] CameraList&                  get_cameras() { return m_cameras; }
        [[nodiscard]] MeshBatchCollector&          get_batch_collector() { return m_batch_collector; }
        [[nodiscard]] MeshBatchCompiler&           get_batch_compiler() { return m_batch_compiler; }
        [[nodiscard]] MeshRenderCmdMerger&         get_cmd_merger() { return m_cmd_merger; }
        [[nodiscard]] RenderCmdAllocator&          get_cmd_allocator() { return m_cmd_allocator; }
        [[nodiscard]] ArrayView<RenderView>        get_views() { return ArrayView<RenderView>(m_views.data(), m_cameras.get_size()); }
        [[nodiscard]] const RenderSettings&        get_settings() const { return m_settings; }
        [[nodiscard]] float                        get_time() const { return m_time; }
        [[nodiscard]] float                        get_delta_time() const { return m_delta_time; }
        [[nodiscard]] const Ref<GfxUniformBuffer>& get_frame_data() const { return m_frame_data; }
        [[nodiscard]] const Ref<GfxVertBuffer>&    get_fullscreen_tria() const { return m_fullscreen_tria.get_buffer(); };

    private:
        std::array<RenderView, RenderLimits::MAX_VIEWS> m_views;
        std::vector<RenderQueue*>                       m_queues;

        MeshBatchCollector  m_batch_collector;
        MeshBatchCompiler   m_batch_compiler;
        MeshRenderCmdMerger m_cmd_merger;
        RenderCmdAllocator  m_cmd_allocator;

        GfxVector<GfxVF_Pos2Uv2, GfxVertBuffer> m_fullscreen_tria;
        Ref<GfxUniformBuffer>                   m_frame_data;

        RenderScene* m_scene = nullptr;
        CameraList   m_cameras;

        RenderSettings m_settings;

        float m_time       = 0.0f;
        float m_delta_time = 0.0f;
        int   m_batch_size = 4;
    };

}// namespace wmoge
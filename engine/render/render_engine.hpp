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

#ifndef WMOGE_RENDER_ENGINE_HPP
#define WMOGE_RENDER_ENGINE_HPP

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "mesh/mesh_batch.hpp"
#include "mesh/mesh_pass.hpp"
#include "platform/window.hpp"
#include "render/render_camera.hpp"
#include "render/render_defs.hpp"
#include "render/render_object.hpp"
#include "render/render_queue.hpp"

#include <array>

namespace wmoge {

    /**
     * @class RenderView
     * @brief Holds data required to render a single view
     */
    struct RenderView {
        static constexpr int QUEUE_COUNT = int(MeshPassType::Total);

        std::array<RenderQueue, QUEUE_COUNT> queues;
        Ref<GfxUniformBuffer>                view_data;
        Ref<GfxDescSet>                      view_set;
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
     * @see MeshBatchCollector
     * @see MeshBatchCompiler
     */
    class RenderEngine {
    public:
        RenderEngine();

        void set_time(float time);
        void set_delta_time(float delta_time);
        void set_target(const Ref<Window>& window);

        void begin_rendering();
        void end_rendering();

        void prepare_frame_data();
        void allocate_veiws();
        void collect_batches(RenderObjectCollector& objects);
        void compile_batches();

        [[nodiscard]] RenderCameras&               get_cameras() { return m_cameras; }
        [[nodiscard]] MeshBatchCollector&          get_collector() { return m_collector; }
        [[nodiscard]] MeshBatchCompiler&           get_compiler() { return m_compiler; }
        [[nodiscard]] ArrayView<RenderView>        get_views() { return ArrayView<RenderView>(m_views.data(), m_cameras.get_size()); }
        [[nodiscard]] float                        get_time() const { return m_time; }
        [[nodiscard]] float                        get_delta_time() const { return m_delta_time; }
        [[nodiscard]] const Ref<Window>&           get_main_target() const { return m_main_target; }
        [[nodiscard]] const Ref<GfxUniformBuffer>& get_frame_data() const { return m_frame_data; }

    private:
        RenderCameras                                   m_cameras;
        MeshBatchCollector                              m_collector;
        MeshBatchCompiler                               m_compiler;
        std::array<RenderView, RenderLimits::MAX_VIEWS> m_views;
        float                                           m_time       = 0.0f;
        float                                           m_delta_time = 0.0f;
        Ref<Window>                                     m_main_target;
        Ref<GfxUniformBuffer>                           m_frame_data;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_ENGINE_HPP

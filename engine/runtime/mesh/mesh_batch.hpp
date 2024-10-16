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
#include "core/buffered_vector.hpp"
#include "core/status.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/aabb.hpp"
#include "math/mat.hpp"
#include "mesh/mesh_pass.hpp"
#include "render/camera.hpp"

#include <array>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class MeshBatch
     * @brief Batch of mesh elements with the same vertex/index buffer and material instances
     */
    struct MeshBatch final {
        Strid               name;                                //< Unique element name for debug
        GfxDrawCall         draw_call;                           //< Params to dispatch a draw
        GfxIndexBufferSetup index_buffer;                        //< Optional index buffer with batch indices
        RenderCameraMask    cam_mask;                            //< Mask in which cameras mesh batch wants to be rendered
        class Material*     material    = nullptr;               //< Material to apply to rendered elements
        class GfxDescSet*   mesh_params = nullptr;               //< Mesh descriptor set with batch common assets
        class MeshPassList* pass_list   = nullptr;               //< Cached list with mesh passes for faster RenderCmd generation
        GfxPrimType         prim_type   = GfxPrimType::Triangles;//< Type of primitives to render
        float               dist        = 0.0f;                  //< Distance from camera for sorting
    };

    static_assert(std::is_trivially_destructible_v<MeshBatch>, "mesh batch must be trivial as possible");

    /**
     * @class MeshBatchCollector
     * @brief Service to collect batches from render objects for drawing
     * 
     * Allows to collect mesh batches (draw requests) from any type of render objects.
     * Mesh batch allows to translate draw request from user code to engine code. 
     * Engine itself can compile batches and work with them in the unified and optimized way.
     */
    class MeshBatchCollector final {
    public:
        MeshBatchCollector();
        MeshBatchCollector(const MeshBatchCollector&) = delete;
        MeshBatchCollector(MeshBatchCollector&&)      = delete;
        ~MeshBatchCollector()                         = default;

        void add_batch(const MeshBatch& batch);
        void clear();

        [[nodiscard]] array_view<const MeshBatch> get_batches() const { return m_batches; }
        [[nodiscard]] int                         get_size() const { return int(m_batches.size()); }
        [[nodiscard]] bool                        is_empty() const { return m_batches.empty(); }

    private:
        std::vector<MeshBatch> m_batches;

        class GfxDriver* m_gfx_driver;

        SpinMutex m_mutex;
    };

    /**
     * @class MeshBatchCompiler
     * @brief Compiles collection of mesh batches into set of render commands for rendering
     * 
     * Compiles mesh batches into a set of render cmds, for each camera and each pass, which pass
     * filtering and relevance options. For batches which supports instancing, compiler assignes
     * bucket slots for further render commands merging.
     */
    class MeshBatchCompiler final {
    public:
        static constexpr int NUM_PASSES_TOTAL = static_cast<int>(MeshPassType::Total);

        MeshBatchCompiler();

        Status compile_batch(const MeshBatch& batch, int batch_index);
        void   set_scene(class RenderScene* scene);
        void   set_views(array_view<struct RenderView> views);
        void   set_cameras(class CameraList& cameras);
        void   set_cmd_allocator(class RenderCmdAllocator& allocator);
        void   clear();

    private:
        std::array<std::unique_ptr<MeshPassProcessor>, NUM_PASSES_TOTAL> m_processors;

        array_view<struct RenderView> m_views;

        class RenderCmdAllocator* m_cmd_allocator  = nullptr;
        class RenderScene*        m_scene          = nullptr;
        class ShaderManager*      m_shader_manager = nullptr;
        class CameraList*         m_cameras        = nullptr;
        class GfxDriver*          m_driver         = nullptr;
    };

}// namespace wmoge
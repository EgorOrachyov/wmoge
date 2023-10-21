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

#ifndef WMOGE_MESH_BATCH_HPP
#define WMOGE_MESH_BATCH_HPP

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/status.hpp"
#include "core/synchronization.hpp"
#include "core/unrolled_list.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_dynamic_buffers.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/aabb.hpp"
#include "math/mat.hpp"
#include "mesh/mesh_pass.hpp"
#include "render/render_camera.hpp"

#include <array>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class MeshBatchElement
     * @brief Single instance of a mesh batch subset with unique transform to draw
     */
    struct MeshBatchElement final {
        StringId    name;     //< Unique element name for debug
        GfxDrawCall draw_call;//< Params to dispatch a draw
    };

    static_assert(std::is_trivially_destructible_v<MeshBatchElement>, "mesh element must be trivial as possible");

    /**
     * @class MeshBatch
     * @brief Batch of mesh elements with the same vertex/index buffer and material instances
     */
    struct MeshBatch final {
        MeshBatchElement     elements[1];                            //< List of batch elements to draw
        GfxIndexBufferSetup  index_buffer;                           //< Optional index buffer with batch indices
        RenderCameraMask     cam_mask;                               //< Mask in which cameras mesh batch wants to be rendered
        class VertexFactory* vertex_factory = nullptr;               //< Vertex factory to provide vertex data and format
        class Material*      material       = nullptr;               //< Material to apply to rendered elements
        class GfxDescSet*    mesh_params    = nullptr;               //< Mesh descriptor set with batch common resources
        class MeshPassList*  pass_list      = nullptr;               //< Cached list with mesh passes for faster RenderCmd generation
        class RenderObject*  object         = nullptr;               //< Render object this batch belongs to
        GfxPrimType          prim_type      = GfxPrimType::Triangles;//< Type of primitives to render
    };

    static_assert(std::is_trivially_destructible_v<MeshBatch>, "mesh batch must be trivial as possible");

    /**
     * @class MeshBatchCollector
     * @brief Service to collect batches from render objects for drawing
     * 
     * Allows to collect mesh batches (draw requests) from any type of render objects.
     * Mesh batch allows to translate draw request from user code to engine code. 
     * Engine itself can compile batches and work with the in the unified and optimized way.
     */
    class MeshBatchCollector final {
    public:
        MeshBatchCollector();
        MeshBatchCollector(const MeshBatchCollector&) = delete;
        MeshBatchCollector(MeshBatchCollector&&)      = delete;
        ~MeshBatchCollector()                         = default;

        void add_batch(const MeshBatch& batch);
        void clear();

        [[nodiscard]] ArrayView<const MeshBatch> get_batches() const { return m_batches; }
        [[nodiscard]] GfxDynVertBuffer*          get_dyn_vbuff() const { return m_dyn_vbuff; }
        [[nodiscard]] GfxDynIndexBuffer*         get_dyn_ibuff() const { return m_dyn_ibuff; }
        [[nodiscard]] GfxDynUniformBuffer*       get_dyn_ubuff() const { return m_dyn_ubuff; }
        [[nodiscard]] int                        get_size() const { return int(m_batches.size()); }
        [[nodiscard]] bool                       is_empty() const { return m_batches.empty(); }

    private:
        std::vector<MeshBatch> m_batches;

        GfxDynVertBuffer*    m_dyn_vbuff;
        GfxDynIndexBuffer*   m_dyn_ibuff;
        GfxDynUniformBuffer* m_dyn_ubuff;
        class GfxDriver*     m_gfx_driver;

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
        void   set_views(ArrayView<struct RenderView> views);
        void   set_cameras(class RenderCameras& cameras);
        void   set_cmd_allocator(class RenderCmdAllocator& allocator);
        void   clear();

    private:
        std::array<std::unique_ptr<MeshPassProcessor>, NUM_PASSES_TOTAL> m_processors;

        ArrayView<struct RenderView> m_views;

        class RenderCmdAllocator* m_cmd_allocator  = nullptr;
        class RenderCameras*      m_cameras        = nullptr;
        class RenderScene*        m_scene          = nullptr;
        class ShaderManager*      m_shader_manager = nullptr;
        class GfxDriver*          m_driver         = nullptr;
        class GfxCtx*             m_ctx            = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_MESH_BATCH_HPP
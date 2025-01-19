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

#include "core/buffered_vector.hpp"
#include "core/flat_set.hpp"
#include "core/mask.hpp"
#include "core/simple_id.hpp"
#include "core/status.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "rdg/rdg_resources.hpp"

#include <functional>

namespace wmoge {

    /** @brief Rdg pass usage flag */
    enum class RdgPassFlag {
        ComputePass,
        GraphicsPass,
        MaterialPass,
        CopyPass
    };

    /** @brief Rdg pass color target info */
    struct RdgPassColorTarget {
        RdgTexture* resource = nullptr;
        Color4f     color    = Color4f();
        bool        clear    = false;
        GfxRtOp     op       = GfxRtOp::LoadStore;
    };

    /** @brief Rdg pass depth stencil target info */
    struct RdgPassDepthTarget {
        RdgTexture* resource      = nullptr;
        float       depth         = 1.0f;
        int         stencil       = 0;
        bool        clear_depth   = false;
        bool        clear_stencil = false;
        GfxRtOp     op            = GfxRtOp::LoadStore;
    };

    /** @brief Rdg pass referenced resource for manual usage */
    struct RdgPassResource {
        RdgResource* resource = nullptr;
        GfxAccess    access   = GfxAccess::None;
    };

    /**
     * @class RdgPassContext
     * @brief Rdg graph execution context passed into pass custom user code
     */
    class RdgPassContext {
    public:
        RdgPassContext(GfxCmdListRef        cmd_list,
                       class GfxDriver*     driver,
                       class ShaderManager* shader_manager,
                       class RdgGraph*      graph);

        Status update_vert_buffer(GfxVertBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data);
        Status update_index_buffer(GfxIndexBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data);
        Status update_uniform_buffer(GfxUniformBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data);
        Status update_storage_buffer(GfxStorageBuffer* buffer, int offset, int range, array_view<const std::uint8_t> data);

        Status validate_param_block(ShaderParamBlock* param_block);

        Status bind_param_block(ShaderParamBlock* param_block);
        Status bind_pso_graphics(Shader* shader, const ShaderPermutation& permutation, const GfxVertElements& vert_elements);
        Status bind_pso_compute(Shader* shader, const ShaderPermutation& permutation);
        Status viewport(const Rect2i& viewport);
        Status bind_vert_buffer(GfxVertBuffer* buffer, int index, int offset = 0);
        Status bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset = 0);
        Status draw(int vertex_count, int base_vertex, int instance_count);
        Status draw_indexed(int index_count, int base_vertex, int instance_count);
        Status dispatch(Vec3i group_count);

        [[nodiscard]] const GfxCmdListRef& get_cmd_list() const { return m_cmd_list; }
        [[nodiscard]] class GfxDriver*     get_driver() const { return m_driver; }
        [[nodiscard]] class ShaderManager* get_shader_manager() const { return m_shader_manager; }
        [[nodiscard]] class RdgGraph*      get_graph() const { return m_graph; }

    private:
        GfxCmdListRef        m_cmd_list;
        class GfxDriver*     m_driver         = nullptr;
        class ShaderManager* m_shader_manager = nullptr;
        class RdgGraph*      m_graph          = nullptr;
    };

    /** @brief Rdg pass flags */
    using RdgPassFlags = Mask<RdgPassFlag>;

    /** @brief Rdg pass id within graph */
    using RdgPassId = SimpleId<std::uint32_t>;

    /** @brief Rdg pass callback called on pass execution */
    using RdgPassCallback = std::function<Status(RdgPassContext& context)>;

    /**
     * @class RdgPass
     * @brief Represents single pass in a rgd graph for execution
    */
    class RdgPass {
    public:
        RdgPass(class RdgGraph& graph, Strid name, RdgPassId id, RdgPassFlags flags);

        RdgPass& color_target(RdgTexture* target);
        RdgPass& color_target(RdgTexture* target, const Color4f& clear_color);
        RdgPass& depth_target(RdgTexture* target);
        RdgPass& depth_target(RdgTexture* target, float clear_depth, int clear_stencil);
        RdgPass& reference(RdgResource* resource, GfxAccess access);
        RdgPass& uniform(RdgBuffer* resource);
        RdgPass& reading(RdgBuffer* resource);
        RdgPass& writing(RdgBuffer* resource);
        RdgPass& copy_source(RdgBuffer* resource);
        RdgPass& copy_destination(RdgBuffer* resource);
        RdgPass& sampling(RdgTexture* resource);
        RdgPass& storage(RdgTexture* resource);
        RdgPass& bind(RdgPassCallback callback);

        GfxRenderPassDesc make_render_pass_desc() const;

        [[nodiscard]] bool                   has_resource(RdgResource* r) const;
        [[nodiscard]] const RdgPassFlags&    get_flags() const { return m_flags; }
        [[nodiscard]] const RdgPassId&       get_id() const { return m_id; }
        [[nodiscard]] const Strid&           get_name() const { return m_name; }
        [[nodiscard]] const RdgPassCallback& get_callback() const { return m_callback; }

    private:
        friend RdgGraph;

        buffered_vector<RdgPassColorTarget, 6> m_color_targets;
        buffered_vector<RdgPassDepthTarget, 1> m_depth_targets;
        buffered_vector<RdgPassResource, 16>   m_resources;
        flat_set<RdgResource*>                 m_referenced;

        RdgPassCallback m_callback;
        RdgPassFlags    m_flags;
        RdgPassId       m_id;
        Strid           m_name;

        class RdgGraph& m_graph;
    };

}// namespace wmoge
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

#ifndef WMOGE_VK_CTX_HPP
#define WMOGE_VK_CTX_HPP

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"

#include "gfx/gfx_ctx.hpp"
#include "gfx/threaded/gfx_ctx_threaded.hpp"
#include "gfx/threaded/gfx_ctx_wrapper.hpp"
#include "gfx/threaded/gfx_worker.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_cmd_manager.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_mem_manager.hpp"
#include "gfx/vulkan/vk_pipeline.hpp"
#include "gfx/vulkan/vk_queues.hpp"
#include "gfx/vulkan/vk_render_pass.hpp"
#include "gfx/vulkan/vk_vert_format.hpp"
#include "gfx/vulkan/vk_window.hpp"

#include <array>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @class VKCtx
     * @brief Vulkan gfx contex implementation
     */
    class VKCtx final : public GfxCtxThreaded {
    public:
        explicit VKCtx(class VKDriver& driver);
        ~VKCtx() override;

        void update_desc_set(const Ref<GfxDescSet>& set, const GfxDescSetResources& resources) override;

        void update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data) override;
        void update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) override;
        void update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data) override;

        void* map_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void* map_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void* map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void* map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;
        void  unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void  unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void  unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void  unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void barrier_image(const Ref<GfxTexture>& texture, GfxTexBarrierType barrier_type) override;
        void barrier_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void begin_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name) override;
        void bind_target(const Ref<Window>& window) override;
        void bind_color_target(const Ref<GfxTexture>& texture, int target, int mip, int slice) override;
        void bind_depth_target(const Ref<GfxTexture>& texture, int mip, int slice) override;
        void viewport(const Rect2i& viewport) override;
        void clear(int target, const Vec4f& color) override;
        void clear(float depth, int stencil) override;
        bool bind_pipeline(const Ref<GfxPipeline>& pipeline) override;
        bool bind_comp_pipeline(const Ref<GfxCompPipeline>& pipeline) override;
        void bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset) override;
        void bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) override;
        void bind_desc_set(const Ref<GfxDescSet>& set, int index) override;
        void bind_desc_sets(const ArrayView<GfxDescSet*>& sets, int offset) override;
        void draw(int vertex_count, int base_vertex, int instance_count) override;
        void draw_indexed(int index_count, int base_vertex, int instance_count) override;
        void dispatch(Vec3i group_count) override;
        void end_render_pass() override;

        void execute(const std::function<void(GfxCtx* thread_ctx)>& functor) override;
        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;

        void begin_label(const Strid& label) override;
        void end_label() override;

        [[nodiscard]] const Mat4x4f&  clip_matrix() const override;
        [[nodiscard]] GfxCtxType      ctx_type() const override;
        [[nodiscard]] CallbackStream* cmd_stream() override;
        [[nodiscard]] bool            check_thread_valid();

    public:
        VkCommandBuffer cmd_begin() { return m_cmd_manager->begin_buffer(); }
        VkCommandBuffer cmd_end() { return m_cmd_manager->end_buffer(); }
        VkCommandBuffer cmd_current() { return m_cmd_manager->current_buffer(); }

    private:
        void prepare_render_pass();

    private:
        std::unique_ptr<VKCmdManager> m_cmd_manager;

        std::unique_ptr<VKRenderPassBinder>                        m_render_pass_binder;
        Ref<VKRenderPass>                                          m_current_pass;
        Ref<VKPipeline>                                            m_current_pipeline;
        Ref<VKCompPipeline>                                        m_current_comp_pipeline;
        Ref<VKShader>                                              m_current_shader;
        Ref<VKIndexBuffer>                                         m_current_index_buffer;
        std::array<Ref<VKVertBuffer>, GfxLimits::MAX_VERT_BUFFERS> m_current_vert_buffers{};
        std::array<int, GfxLimits::MAX_VERT_BUFFERS>               m_current_vert_buffers_offsets{};
        std::array<VkDescriptorSet, GfxLimits::MAX_DESC_SETS>      m_desc_sets{};
        std::array<Vec4f, GfxLimits::MAX_COLOR_TARGETS>            m_clear_color;
        float                                                      m_clear_depth   = 1.0f;
        int                                                        m_clear_stencil = 0;
        Rect2i                                                     m_viewport;

        bool  m_in_render_pass      = false;
        bool  m_render_pass_started = false;
        bool  m_pipeline_bound      = false;
        bool  m_comp_pipeline_bound = false;
        bool  m_target_bound        = false;
        Strid m_render_pass_name;

        GfxCtxType m_ctx_type = GfxCtxType::Immediate;
        Mat4x4f    m_clip_matrix;

        class VKDriver& m_driver;
    };

}// namespace wmoge

#endif//WMOGE_VK_CTX_HPP

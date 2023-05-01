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

#ifndef WMOGE_VK_DRIVER_HPP
#define WMOGE_VK_DRIVER_HPP

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"

#include "gfx/gfx_driver.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_desc_manager.hpp"
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
     * @class VKDriver
     * @brief Vulkan gfx driver implementation
     */
    class VKDriver final : public GfxDriver {
    public:
        VKDriver(VKInitInfo info);
        ~VKDriver() override;

        ref_ptr<GfxVertFormat>    make_vert_format(const GfxVertElements& elements, const StringId& name) override;
        ref_ptr<GfxVertBuffer>    make_vert_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        ref_ptr<GfxIndexBuffer>   make_index_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        ref_ptr<GfxUniformBuffer> make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        ref_ptr<GfxStorageBuffer> make_storage_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        ref_ptr<GfxShader>        make_shader(std::string vertex, std::string fragment, const StringId& name) override;
        ref_ptr<GfxShader>        make_shader(ref_ptr<Data> code, const StringId& name) override;
        ref_ptr<GfxTexture>       make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        ref_ptr<GfxTexture>       make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        ref_ptr<GfxTexture>       make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        ref_ptr<GfxSampler>       make_sampler(const GfxSamplerDesc& desc, const StringId& name) override;
        ref_ptr<GfxRenderPass>    make_render_pass(GfxRenderPassType pass_type, const StringId& name) override;
        ref_ptr<GfxPipeline>      make_pipeline(const GfxPipelineState& state, const StringId& name) override;

        void update_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) override;
        void update_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) override;
        void update_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) override;
        void update_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) override;
        void update_texture_2d(const ref_ptr<GfxTexture>& texture, int mip, Rect2i region, const ref_ptr<Data>& data) override;
        void update_texture_2d_array(const ref_ptr<GfxTexture>& texture, int mip, int slice, Rect2i region, const ref_ptr<Data>& data) override;
        void update_texture_cube(const ref_ptr<GfxTexture>& texture, int mip, int face, Rect2i region, const ref_ptr<Data>& data) override;

        void* map_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer) override;
        void* map_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer) override;
        void* map_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer) override;
        void* map_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer) override;
        void  unmap_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer) override;
        void  unmap_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer) override;
        void  unmap_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer) override;
        void  unmap_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer) override;

        void begin_render_pass(const ref_ptr<GfxRenderPass>& pass) override;
        void bind_target(const ref_ptr<Window>& window) override;
        void bind_color_target(const ref_ptr<GfxTexture>& texture, int target, int mip, int slice) override;
        void bind_depth_target(const ref_ptr<GfxTexture>& texture, int mip, int slice) override;
        void viewport(const Rect2i& viewport) override;
        void clear(int target, const Vec4f& color) override;
        void clear(float depth, int stencil) override;
        bool bind_pipeline(const ref_ptr<GfxPipeline>& pipeline) override;
        void bind_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer, int index, int offset) override;
        void bind_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) override;
        void bind_texture(const StringId& name, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) override;
        void bind_texture(const GfxLocation& location, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) override;
        void bind_uniform_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer) override;
        void bind_uniform_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer) override;
        void bind_storage_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer) override;
        void bind_storage_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer) override;
        void draw(int vertex_count, int base_vertex, int instance_count) override;
        void draw_indexed(int index_count, int base_vertex, int instance_count) override;
        void end_render_pass() override;

        void shutdown() override;
        void begin_frame() override;
        void flush() override;
        void end_frame() override;
        void prepare_window(const ref_ptr<Window>& window) override;
        void swap_buffers(const ref_ptr<Window>& window) override;

        template<typename Callback>
        void push_deferred_release(Callback&& release);

    private:
        void init_functions();
        void init_instance();
        void init_physical_device_and_queues(VKWindow& window);
        void init_device();
        void init_glslang();
        void init_pipeline_cache();
        void release_pipeline_cache();
        void init_context();
        void release_context();
        bool supports(const std::string& extension);

        bool use_direct_update() { return on_gfx_thread() && m_cmd; }

        void prepare_draw();
        void prepare_render_pass();
        void release_resources(uint64_t index);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                             VkDebugUtilsMessageTypeFlagsEXT             message_type,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                                                             void*                                       p_user_data);

    public:
        VkInstance       instance() { return m_instance; }
        VkPhysicalDevice phys_device() { return m_phys_device; }
        VkDevice         device() { return m_device; }
        VkPipelineCache  pipeline_cache() { return m_pipeline_cache; }

        VkCommandBuffer cmd() { return m_cmd; }

        VKWindowManager* window_manager() { return m_window_manager.get(); }
        VKQueues*        queues() { return m_queues.get(); }
        VKMemManager*    mem_manager() { return m_mem_manager.get(); }

    private:
        std::string              m_app_name;
        std::string              m_engine_name;
        std::vector<std::string> m_required_layers;
        std::vector<std::string> m_required_extensions;
        std::vector<std::string> m_required_device_extensions;
        bool                     m_use_validation = true;

        VkInstance               m_instance        = VK_NULL_HANDLE;
        VkPhysicalDevice         m_phys_device     = VK_NULL_HANDLE;
        VkDevice                 m_device          = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
        VkPipelineCache          m_pipeline_cache  = VK_NULL_HANDLE;

        VkCommandBuffer m_cmd = VK_NULL_HANDLE;

        std::unique_ptr<VKWindowManager>   m_window_manager;
        std::unique_ptr<VKQueues>          m_queues;
        std::unique_ptr<VKMemManager>      m_mem_manager;
        std::unique_ptr<VKDescManager>     m_desc_manager;
        std::vector<VkExtensionProperties> m_device_extensions;

        fast_map<GfxVertElements, ref_ptr<VKVertFormat>> m_formats;
        fast_map<GfxSamplerDesc, ref_ptr<VKSampler>>     m_samplers;
        fast_map<GfxPipelineState, ref_ptr<VKPipeline>>  m_pipelines;
        std::mutex                                       m_cached_mutex;

        ref_ptr<VKRenderPass>                                          m_current_pass;
        ref_ptr<VKPipeline>                                            m_current_pipeline;
        ref_ptr<VKShader>                                              m_current_shader;
        ref_ptr<VKIndexBuffer>                                         m_current_index_buffer;
        std::array<ref_ptr<VKVertBuffer>, GfxLimits::MAX_VERT_BUFFERS> m_current_vert_buffers{};
        std::array<int, GfxLimits::MAX_VERT_BUFFERS>                   m_current_vert_buffers_offsets{};
        std::unordered_set<ref_ptr<VKWindow>>                          m_to_present;
        std::array<Vec4f, GfxLimits::MAX_COLOR_TARGETS>                m_clear_color;
        float                                                          m_clear_depth   = 1.0f;
        int                                                            m_clear_stencil = 0;
        Rect2i                                                         m_viewport;

        bool m_in_render_pass      = false;
        bool m_render_pass_started = false;
        bool m_pipeline_bound      = false;
        bool m_target_bound        = false;

        bool m_shutdown = false;

        std::array<VkCommandBuffer, GfxLimits::FRAMES_IN_FLIGHT> m_cmds{};
        std::array<VkCommandPool, GfxLimits::FRAMES_IN_FLIGHT>   m_cmds_pools{};
        std::array<VkFence, GfxLimits::FRAMES_IN_FLIGHT>         m_fences{};
        std::array<VkSemaphore, GfxLimits::FRAMES_IN_FLIGHT>     m_rendering_finished{};
        std::array<CallbackQueue, GfxLimits::FRAMES_IN_FLIGHT>   m_deferred_release;

        std::size_t m_index = 0 % GfxLimits::FRAMES_IN_FLIGHT;
    };

    template<typename Callback>
    void VKDriver::push_deferred_release(Callback&& release) {
        if (on_gfx_thread()) {
            if (m_shutdown) {
                // If on gfx thread and in shutdown mode can release here
                release();
            } else {
                // Else deferred release since it can be used by gpu
                m_deferred_release[m_index].push(std::forward<Callback>(release));
            }
        } else {
            // Wrap release function
            auto wrapped_release = [r = release, this]() mutable {
                m_deferred_release[m_index].push(std::move(r));
            };

            // Push through MT queue for deletion later
            queue()->push(std::move(wrapped_release));
        }
    }

}// namespace wmoge

#endif//WMOGE_VK_DRIVER_HPP

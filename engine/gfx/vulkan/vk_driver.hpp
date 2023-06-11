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
#include "gfx/threaded/gfx_driver_threaded.hpp"
#include "gfx/threaded/gfx_driver_wrapper.hpp"
#include "gfx/threaded/gfx_worker.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_ctx.hpp"
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
    class VKDriver final : public GfxDriverThreaded {
    public:
        explicit VKDriver(const VKInitInfo& info);
        ~VKDriver() override;

        Ref<GfxVertFormat>       make_vert_format(const GfxVertElements& elements, const StringId& name) override;
        Ref<GfxVertBuffer>       make_vert_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxIndexBuffer>      make_index_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxUniformBuffer>    make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxStorageBuffer>    make_storage_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxShader>           make_shader(std::string vertex, std::string fragment, const StringId& name) override;
        Ref<GfxShader>           make_shader(Ref<Data> code, const StringId& name) override;
        Ref<GfxTexture>          make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxTexture>          make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxTexture>          make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxSampler>          make_sampler(const GfxSamplerDesc& desc, const StringId& name) override;
        Ref<GfxPipeline>         make_pipeline(const GfxPipelineState& state, const StringId& name) override;
        Ref<GfxRenderPass>       make_render_pass(const GfxRenderPassDesc& pass_desc, const StringId& name) override;
        Ref<GfxDynVertBuffer>    make_dyn_vert_buffer(int chunk_size, const StringId& name) override;
        Ref<GfxDynIndexBuffer>   make_dyn_index_buffer(int chunk_size, const StringId& name) override;
        Ref<GfxDynUniformBuffer> make_dyn_uniform_buffer(int chunk_size, const StringId& name) override;

        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;
        void prepare_window(const Ref<Window>& window) override;
        void swap_buffers(const Ref<Window>& window) override;

        class GfxCtx* ctx_immediate() override { return m_ctx_immediate.get(); }
        class GfxCtx* ctx_async() override { return m_ctx_async.get(); }

        GfxDynVertBuffer*    dyn_vert_buffer() override { return m_dyn_vert_buffer.get(); }
        GfxDynIndexBuffer*   dyn_index_buffer() override { return m_dyn_index_buffer.get(); }
        GfxDynUniformBuffer* dyn_uniform_buffer() override { return m_dyn_uniform_buffer.get(); }

        const GfxDeviceCaps&   device_caps() const override { return m_device_caps; }
        const StringId&        driver_name() const override { return m_driver_name; }
        const std::string&     shader_cache_path() const override { return m_shader_cache_path; }
        const std::string&     pipeline_cache_path() const override { return m_pipeline_cache_path; }
        const std::thread::id& thread_id() const override { return m_thread_id; }
        const Mat4x4f&         clip_matrix() const override { return m_clip_matrix; }
        std::size_t            frame_number() const override { return m_frame_number.load(); }
        bool                   on_gfx_thread() const override { return m_thread_id == std::this_thread::get_id(); }
        GfxShaderLang          shader_lang() const override { return GfxShaderLang::GlslVk450; }
        CmdStream*             cmd_stream() override { return m_driver_cmd_stream.get(); }
        GfxDriverWrapper*      driver_wrapper() { return m_driver_wrapper.get(); }
        GfxCtxWrapper*         ctx_immediate_wrapper() { return m_ctx_immediate_wrapper.get(); }
        CallbackQueue*         release_queue() { return &m_deferred_release[m_index]; }

    public:
        VkInstance       instance() { return m_instance; }
        VkPhysicalDevice phys_device() { return m_phys_device; }
        VkDevice         device() { return m_device; }
        VkPipelineCache  pipeline_cache() { return m_pipeline_cache; }
        VKWindowManager* window_manager() { return m_window_manager.get(); }
        VKQueues*        queues() { return m_queues.get(); }
        VKMemManager*    mem_manager() { return m_mem_manager.get(); }
        VKCtx*           vk_ctx() { return m_ctx_immediate.get(); }

    private:
        void init_functions();
        void init_instance();
        void init_physical_device_and_queues(VKWindow& window);
        void init_device();
        void init_glslang();
        void init_pipeline_cache();
        void release_pipeline_cache();
        void init_sync_fences();
        void release_sync_fences();
        bool supports(const std::string& extension);
        void release_resources(uint64_t index);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                                                             VkDebugUtilsMessageTypeFlagsEXT             message_type,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
                                                             void*                                       p_user_data);

    private:
        VkInstance               m_instance        = VK_NULL_HANDLE;
        VkPhysicalDevice         m_phys_device     = VK_NULL_HANDLE;
        VkDevice                 m_device          = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
        VkPipelineCache          m_pipeline_cache  = VK_NULL_HANDLE;

        std::size_t m_index = 0 % GfxLimits::FRAMES_IN_FLIGHT;

        std::array<CallbackQueue, GfxLimits::FRAMES_IN_FLIGHT> m_deferred_release;
        std::vector<Ref<VKWindow>>                             m_to_present;
        std::vector<VkSemaphore>                               m_queue_wait;
        std::vector<VkSemaphore>                               m_queue_signal;
        VkFence                                                m_sync_fence = VK_NULL_HANDLE;

        Ref<GfxDynVertBuffer>    m_dyn_vert_buffer;
        Ref<GfxDynIndexBuffer>   m_dyn_index_buffer;
        Ref<GfxDynUniformBuffer> m_dyn_uniform_buffer;

        fast_map<GfxVertElements, Ref<VKVertFormat>>   m_formats;
        fast_map<GfxSamplerDesc, Ref<VKSampler>>       m_samplers;
        fast_map<GfxPipelineState, Ref<VKPipeline>>    m_pipelines;
        fast_map<GfxRenderPassDesc, Ref<VKRenderPass>> m_render_passes;

        GfxDeviceCaps      m_device_caps;
        StringId           m_driver_name = SID("unknown");
        std::thread::id    m_thread_id   = std::this_thread::get_id();
        Mat4x4f            m_clip_matrix;
        std::atomic_size_t m_frame_number{0};
        std::string        m_shader_cache_path;
        std::string        m_pipeline_cache_path;

        std::string              m_app_name;
        std::string              m_engine_name;
        std::vector<std::string> m_required_layers;
        std::vector<std::string> m_required_extensions;
        std::vector<std::string> m_required_device_extensions;
        bool                     m_use_validation = true;

        std::unique_ptr<GfxDriverWrapper>  m_driver_wrapper;
        std::unique_ptr<GfxCtxWrapper>     m_ctx_immediate_wrapper;
        std::unique_ptr<GfxWorker>         m_driver_worker;
        std::unique_ptr<CmdStream>         m_driver_cmd_stream;
        std::unique_ptr<VKWindowManager>   m_window_manager;
        std::unique_ptr<VKQueues>          m_queues;
        std::unique_ptr<VKMemManager>      m_mem_manager;
        std::unique_ptr<VKCtx>             m_ctx_immediate;
        std::unique_ptr<VKCtx>             m_ctx_async;
        std::vector<VkExtensionProperties> m_device_extensions;
    };

}// namespace wmoge

#endif//WMOGE_VK_DRIVER_HPP

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
#include "core/flat_map.hpp"
#include "core/task_manager.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/threaded/gfx_driver_threaded.hpp"
#include "gfx/threaded/gfx_driver_wrapper.hpp"
#include "gfx/threaded/gfx_worker.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_ctx.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_desc_manager.hpp"
#include "gfx/vulkan/vk_desc_set.hpp"
#include "gfx/vulkan/vk_mem_manager.hpp"
#include "gfx/vulkan/vk_pipeline.hpp"
#include "gfx/vulkan/vk_queues.hpp"
#include "gfx/vulkan/vk_render_pass.hpp"
#include "gfx/vulkan/vk_sampler.hpp"
#include "gfx/vulkan/vk_vert_format.hpp"
#include "gfx/vulkan/vk_window.hpp"
#include "platform/file_system.hpp"

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

        Ref<GfxVertFormat>       make_vert_format(const GfxVertElements& elements, const Strid& name) override;
        Ref<GfxVertBuffer>       make_vert_buffer(int size, GfxMemUsage usage, const Strid& name) override;
        Ref<GfxIndexBuffer>      make_index_buffer(int size, GfxMemUsage usage, const Strid& name) override;
        Ref<GfxUniformBuffer>    make_uniform_buffer(int size, GfxMemUsage usage, const Strid& name) override;
        Ref<GfxStorageBuffer>    make_storage_buffer(int size, GfxMemUsage usage, const Strid& name) override;
        Ref<GfxShader>           make_shader(GfxShaderDesc desc, const Strid& name) override;
        Ref<GfxShaderProgram>    make_program(GfxShaderProgramDesc desc, const Strid& name) override;
        Ref<GfxTexture>          make_texture(const GfxTextureDesc& desc, const Strid& name) override;
        Ref<GfxTexture>          make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, GfxTexSwizz swizz, const Strid& name) override;
        Ref<GfxTexture>          make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) override;
        Ref<GfxTexture>          make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) override;
        Ref<GfxSampler>          make_sampler(const GfxSamplerDesc& desc, const Strid& name) override;
        Ref<GfxPsoLayout>        make_pso_layout(const GfxDescSetLayouts& layouts, const Strid& name) override;
        Ref<GfxPsoGraphics>      make_pso_graphics(const GfxPsoStateGraphics& state, const Strid& name) override;
        Ref<GfxPsoCompute>       make_pso_compute(const GfxPsoStateCompute& state, const Strid& name) override;
        Ref<GfxRenderPass>       make_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name) override;
        Ref<VKFramebufferObject> make_frame_buffer(const VKFrameBufferDesc& desc, const Strid& name);
        Ref<GfxDynVertBuffer>    make_dyn_vert_buffer(int chunk_size, const Strid& name) override;
        Ref<GfxDynIndexBuffer>   make_dyn_index_buffer(int chunk_size, const Strid& name) override;
        Ref<GfxDynUniformBuffer> make_dyn_uniform_buffer(int chunk_size, const Strid& name) override;
        Ref<GfxDescSetLayout>    make_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name) override;
        Ref<GfxDescSet>          make_desc_set(const GfxDescSetResources& resources, const Ref<GfxDescSetLayout>& layout, const Strid& name) override;
        Async                    make_shaders(const Ref<GfxAsyncShaderRequest>& request) override;
        Async                    make_psos_graphics(const Ref<GfxAsyncPsoRequestGraphics>& request) override;
        Async                    make_psos_compute(const Ref<GfxAsyncPsoRequestCompute>& request) override;

        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;
        void prepare_window(const Ref<Window>& window) override;
        void swap_buffers(const Ref<Window>& window) override;

        class GfxCtx* ctx_immediate() override { return m_ctx_immediate.get(); }
        class GfxCtx* ctx_async() override { return m_ctx_async.get(); }

        const GfxDeviceCaps&   device_caps() const override { return m_device_caps; }
        const Strid&           driver_name() const override { return m_driver_name; }
        const std::string&     pipeline_cache_path() const override { return m_pipeline_cache_path; }
        const std::thread::id& thread_id() const override { return m_thread_id; }
        const Mat4x4f&         clip_matrix() const override { return m_clip_matrix; }
        std::size_t            frame_number() const override { return m_frame_number.load(); }
        bool                   on_gfx_thread() const override { return m_thread_id == std::this_thread::get_id(); }
        GfxShaderLang          shader_lang() const override { return GfxShaderLang::GlslVk450; }
        CallbackStream*        cmd_stream() override { return m_driver_cmd_stream.get(); }
        GfxType                get_gfx_type() const override { return GfxType::Vulkan; }
        GfxShaderPlatform      get_shader_platform() const override { return m_shader_patform; }
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
        VKDescManager*   desc_manager() { return m_desc_manager.get(); }
        VKCtx*           vk_ctx() { return m_ctx_immediate.get(); }

    private:
        void init_functions();
        void init_instance();
        void init_physical_device_and_queues(VKWindow& window);
        void init_device();
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

        flat_map<GfxSamplerDesc, Ref<VKSampler>>              m_samplers;
        flat_map<GfxRenderPassDesc, Ref<VKRenderPass>>        m_render_passes;
        flat_map<VKFrameBufferDesc, Ref<VKFramebufferObject>> m_frame_buffers;

        GfxDeviceCaps      m_device_caps;
        Strid              m_driver_name = SID("unknown");
        std::thread::id    m_thread_id   = std::this_thread::get_id();
        Mat4x4f            m_clip_matrix;
        std::atomic_size_t m_frame_number{0};
        std::string        m_pipeline_cache_path;
        GfxShaderPlatform  m_shader_patform = GfxShaderPlatform::None;

        std::string              m_app_name;
        std::string              m_engine_name;
        std::vector<std::string> m_required_layers;
        std::vector<std::string> m_required_extensions;
        std::vector<std::string> m_required_device_extensions;
        bool                     m_use_validation = false;

        std::unique_ptr<GfxDriverWrapper>  m_driver_wrapper;
        std::unique_ptr<GfxCtxWrapper>     m_ctx_immediate_wrapper;
        std::unique_ptr<GfxWorker>         m_driver_worker;
        std::unique_ptr<CallbackStream>    m_driver_cmd_stream;
        std::unique_ptr<VKWindowManager>   m_window_manager;
        std::unique_ptr<VKQueues>          m_queues;
        std::unique_ptr<VKMemManager>      m_mem_manager;
        std::unique_ptr<VKDescManager>     m_desc_manager;
        std::unique_ptr<VKCtx>             m_ctx_immediate;
        std::unique_ptr<VKCtx>             m_ctx_async;
        std::vector<VkExtensionProperties> m_device_extensions;

        FileSystem*  m_file_system  = nullptr;
        TaskManager* m_task_manager = nullptr;
    };

}// namespace wmoge
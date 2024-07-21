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

#include "vk_driver.hpp"

#include "core/string_utils.hpp"
#include "core/task.hpp"
#include "core/task_parallel_for.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_cmd_list.hpp"
#include "gfx/vulkan/vk_desc_set.hpp"
#include "gfx/vulkan/vk_pipeline.hpp"
#include "gfx/vulkan/vk_sampler.hpp"
#include "gfx/vulkan/vk_shader.hpp"
#include "gfx/vulkan/vk_texture.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <cassert>
#include <fstream>

namespace wmoge {

    std::vector<const char*> pack_strings(const std::vector<std::string>& names) {
        std::vector<const char*> names_p(names.size());
        for (int i = 0; i < names.size(); i++) names_p[i] = names[i].c_str();
        return names_p;
    };

    VKDriver::VKDriver(const VKInitInfo& info) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::VKDriver");

        Config* config = IocContainer::iresolve_v<Config>();

        m_file_system  = IocContainer::iresolve_v<FileSystem>();
        m_task_manager = IocContainer::iresolve_v<TaskManager>();

        m_driver_name = SID("vulkan");
        m_clip_matrix = Mat4x4f(1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.5f, 0.5f,
                                0.0f, 0.0f, 0.0f, 1.0f);

        m_app_name            = info.app_name;
        m_engine_name         = info.engine_name;
        m_required_extensions = info.required_ext;

#ifdef TARGET_WINDOWS
        m_shader_patform = GfxShaderPlatform::VulkanWindows;
#endif
#ifdef TARGET_LINUX
        m_shader_patform = GfxShaderPlatform::VulkanLinux;
#endif
#ifdef TARGET_MACOS
        m_shader_patform = GfxShaderPlatform::VulkanMacOS;
#endif

#ifndef WMOGE_RELEASE
        m_use_validation = config->get_bool_or_default(SID("gfx.vulkan.validation_layer"), true);
#endif

        if (m_use_validation) {
            m_required_layers.emplace_back("VK_LAYER_KHRONOS_validation");
            WG_LOG_INFO("request " << VK_LAYER_KHRONOS_VALIDATION);

            m_required_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            WG_LOG_INFO("request " << VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        m_required_device_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        WG_LOG_INFO("request " << VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        m_pipeline_cache_path = config->get_string_or_default(SID("gfx.vulkan.pipeline_cache"), "cache://pipelines_vk.cache");

        // load vulkan functions from volk
        init_functions();

        // instance, validation layers and debug callbacks setup
        init_instance();

        // create tmp surface for physical device selection
        VkSurfaceKHR surface;
        WG_VK_CHECK(info.factory(m_instance, info.window, surface));
        VKWindow window(info.window, surface, *this);

        // select physical device and find queues
        init_physical_device_and_queues(window);

        // create logical device with all required features
        init_device();

        // init queues after device creation
        m_queues->init_queues(m_device);

        // create surface manager for swap chains
        m_window_manager = std::make_unique<VKWindowManager>(info, *this);

        // init mem manager for allocations
        m_mem_manager = std::make_unique<VKMemManager>(*this);

        // init semaphores pool for cmd buffers sync
        m_semaphore_pool = std::make_unique<VKSemaphorePool>(*this);

        // init cmd manager
        m_cmd_manager = std::make_unique<VKCmdManager>(*this);

        // init manager for desc sets allocation
        VKDescPoolConfig pool_config{};
        config->get_int(SID("gfx.vulkan.desc_pool_max_images"), pool_config.max_images);
        config->get_int(SID("gfx.vulkan.desc_pool_max_ub"), pool_config.max_ub);
        config->get_int(SID("gfx.vulkan.desc_pool_max_sb"), pool_config.max_sb);
        config->get_int(SID("gfx.vulkan.desc_pool_max_sets"), pool_config.max_sets);
        m_desc_manager = std::make_unique<VKDescManager>(pool_config, *this);

        // init pipeline cache
        init_pipeline_cache();

        // sync primitives
        init_sync_fences();

        WG_LOG_INFO("init vulkan gfx driver");
    }
    VKDriver::~VKDriver() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::~VKDriver");

        shutdown();

        WG_LOG_INFO("shutdown vulkan gfx driver");
    }

    void VKDriver::shutdown() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::shutdown");

        auto flush_release = [&]() {
            for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
                release_resources(i);
            }
        };

        if (m_instance) {
            WG_VK_CHECK(vkDeviceWaitIdle(m_device));

            m_cmd_manager.reset();
            flush_release();

            m_render_passes.clear();
            flush_release();

            m_desc_manager.reset();
            flush_release();

            m_samplers.clear();
            flush_release();

            m_window_manager.reset();
            flush_release();

            flush_release();
            release_sync_fences();
            release_pipeline_cache();

            m_semaphore_pool.reset();
            m_mem_manager.reset();
            m_queues.reset();

            if (m_device) {
                vkDestroyDevice(m_device, nullptr);
            }

            if (m_debug_messenger) {
                VKDebug::vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
            }

            vkDestroyInstance(m_instance, nullptr);

            m_device      = VK_NULL_HANDLE;
            m_phys_device = VK_NULL_HANDLE;
            m_instance    = VK_NULL_HANDLE;
        }
    }

    Ref<GfxVertFormat> VKDriver::make_vert_format(const GfxVertElements& elements, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_vert_format");

        return make_ref<VKVertFormat>(elements, name);
    }
    Ref<GfxVertBuffer> VKDriver::make_vert_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_vert_buffer");

        auto buffer = make_ref<VKVertBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    Ref<GfxIndexBuffer> VKDriver::make_index_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_index_buffer");

        auto buffer = make_ref<VKIndexBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    Ref<GfxUniformBuffer> VKDriver::make_uniform_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_uniform_buffer");

        auto buffer = make_ref<VKUniformBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    Ref<GfxStorageBuffer> VKDriver::make_storage_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_storage_buffer");

        auto buffer = make_ref<VKStorageBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    Ref<GfxShader> VKDriver::make_shader(GfxShaderDesc desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_shader");

        auto shader = make_ref<VKShader>(name, *this);
        shader->create(std::move(desc));
        return shader;
    }
    Ref<GfxShaderProgram> VKDriver::make_program(GfxShaderProgramDesc desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_program");

        auto program = make_ref<VKShaderProgram>(name, *this);
        program->create(std::move(desc));
        return program;
    }

    Ref<GfxTexture> VKDriver::make_texture(const GfxTextureDesc& desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_texture");

        auto texture = make_ref<VKTexture>(*this);
        texture->create(desc, name);
        return texture;
    }

    Ref<GfxTexture> VKDriver::make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, GfxTexSwizz swizz, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_texture_2d");

        auto texture = make_ref<VKTexture>(*this);
        texture->create_2d(width, height, mips, format, usages, mem_usage, swizz, name);
        return texture;
    }
    Ref<GfxTexture> VKDriver::make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_texture_2d_array");

        auto texture = make_ref<VKTexture>(*this);
        texture->create_2d_array(width, height, mips, slices, format, usages, mem_usage, name);
        return texture;
    }
    Ref<GfxTexture> VKDriver::make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_texture_cube");

        auto texture = make_ref<VKTexture>(*this);
        texture->create_cube(width, height, mips, format, usages, mem_usage, name);
        return texture;
    }
    Ref<GfxSampler> VKDriver::make_sampler(const GfxSamplerDesc& desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_sampler");

        auto& sampler = m_samplers[desc];
        if (!sampler) {
            sampler = make_ref<VKSampler>(desc, name, *this);
            sampler->create();

            WG_LOG_INFO("cache new sampler " << name);
        }

        return sampler;
    }
    Ref<GfxPsoLayout> VKDriver::make_pso_layout(const GfxDescSetLayouts& layouts, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_pso_layout");

        return make_ref<VKPsoLayout>(layouts, name, *this);
    }
    Ref<GfxPsoGraphics> VKDriver::make_pso_graphics(const GfxPsoStateGraphics& state, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_pso_graphics");

        Ref<VKPsoGraphics> pipeline = make_ref<VKPsoGraphics>(name, *this);
        return pipeline->compile(state) ? pipeline : Ref<VKPsoGraphics>();
    }
    Ref<GfxPsoCompute> VKDriver::make_pso_compute(const GfxPsoStateCompute& state, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_pso_compute");

        Ref<VKPsoCompute> pipeline = make_ref<VKPsoCompute>(name, *this);
        return pipeline->compile(state) ? pipeline : Ref<VKPsoCompute>();
    }
    Ref<GfxRenderPass> VKDriver::make_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_render_pass");

        auto& render_pass = m_render_passes[pass_desc];
        if (!render_pass) {
            render_pass = make_ref<VKRenderPass>(pass_desc, name, *this);
            WG_LOG_INFO("cache new render pass " << name);
        }

        return render_pass;
    }
    Ref<GfxRenderPass> VKDriver::make_render_pass(const Ref<Window>& window, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_render_pass");

        return Ref<GfxRenderPass>();
    }
    Ref<GfxFrameBuffer> VKDriver::make_frame_buffer(const GfxFrameBufferDesc& desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_frame_buffer");

        return make_ref<VKFrameBuffer>(desc, name, *this);
    }
    Ref<GfxDescSetLayout> VKDriver::make_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_desc_layout");

        return make_ref<VKDescSetLayout>(desc, name, *this);
    }
    Ref<GfxDescSet> VKDriver::make_desc_set(const GfxDescSetResources& resources, const Ref<GfxDescSetLayout>& layout, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_desc_set");

        assert(layout);

        return make_ref<VKDescSet>(resources, layout.cast<VKDescSetLayout>(), name, *this);
    }
    Async VKDriver::make_shaders(const Ref<GfxAsyncShaderRequest>& request) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_shaders");

        request->shaders.resize(request->desc.size());

        TaskParallelFor task(SID("make_shaders"), [r = request, this](TaskContext&, int item_id, int) {
            Ref<VKShader> shader = make_ref<VKShader>(r->names[item_id], *this);
            shader->create(r->desc[item_id]);
            r->shaders[item_id] = shader;
            return 0;
        });

        return task.schedule(int(request->desc.size()), 1).as_async();
    }
    Async VKDriver::make_psos_graphics(const Ref<GfxAsyncPsoRequestGraphics>& request) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_psos_graphics");

        request->pso.resize(request->states.size());

        TaskParallelFor task(SID("make_psos_graphics"), [r = request, this](TaskContext&, int item_id, int) {
            Ref<VKPsoGraphics> pso = make_ref<VKPsoGraphics>(r->names[item_id], *this);
            if (pso->compile(r->states[item_id])) {
                r->pso[item_id] = pso;
            }
            return 0;
        });

        return task.schedule(int(request->states.size()), 1).as_async();
    }
    Async VKDriver::make_psos_compute(const Ref<GfxAsyncPsoRequestCompute>& request) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::make_psos_compute");

        request->pso.resize(request->states.size());

        TaskParallelFor task(SID("make_psos_compute"), [r = request, this](TaskContext&, int item_id, int) {
            Ref<VKPsoCompute> pso = make_ref<VKPsoCompute>(r->names[item_id], *this);
            if (pso->compile(r->states[item_id])) {
                r->pso[item_id] = pso;
            }
            return 0;
        });

        task.set_task_manager(*m_task_manager);

        return task.schedule(int(request->states.size()), 1).as_async();
    }

    void VKDriver::begin_frame(std::size_t frame_id, const array_view<Ref<Window>>& windows) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::begin_frame");

        m_frame_id = frame_id;
        m_index    = m_frame_id % GfxLimits::FRAMES_IN_FLIGHT;

        release_resources(m_index);

        m_cmd_manager->update(m_frame_id);
        m_mem_manager->update(m_frame_id);
        m_semaphore_pool->update(m_frame_id);

        VkCommandBuffer cmd_buffer = m_cmd_manager->allocate(GfxQueueType::Graphics);

        for (auto& window : windows) {
            auto vk_window = m_window_manager->get_or_create(window);
            vk_window->acquire_next();
            vk_window->color()[vk_window->current()]->transition_layout(cmd_buffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            m_to_present.push_back(vk_window);
        }

        m_cmd_manager->submit(GfxQueueType::Graphics, cmd_buffer);
    }

    GfxCmdListRef VKDriver::acquire_cmd_list(GfxQueueType queue_type) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::acquire_cmd_list");

        return make_ref<VKCmdList>(m_cmd_manager->allocate(queue_type), queue_type, *this);
    }

    void VKDriver::submit_cmd_list(const GfxCmdListRef& cmd_list) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::submit_cmd_list");

        assert(cmd_list);

        VKCmdList* vk_cmd_list = dynamic_cast<VKCmdList*>(cmd_list.get());
        m_cmd_manager->submit(vk_cmd_list->get_queue_type(), vk_cmd_list->get_handle());
    }

    void VKDriver::end_frame(bool swap_buffers) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::end_frame");

        VkCommandBuffer cmd_buffer = m_cmd_manager->allocate(GfxQueueType::Graphics);

        buffered_vector<VkSemaphore> queue_wait;
        buffered_vector<VkSemaphore> queue_signal;

        for (auto& window : m_to_present) {
            window->color()[window->current()]->transition_layout(cmd_buffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            queue_wait.push_back(window->acquire_semaphore());
            queue_signal.push_back(window->present_semaphore());
        }

        WG_VK_CHECK(vkWaitForFences(m_device, 1, &m_sync_fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
        WG_VK_CHECK(vkResetFences(m_device, 1, &m_sync_fence));

        m_cmd_manager->submit(GfxQueueType::Graphics, cmd_buffer, {}, {}, m_sync_fence);
        m_cmd_manager->flush(queue_wait, queue_signal);

        if (swap_buffers) {
            buffered_vector<VkSwapchainKHR> swapchains;
            buffered_vector<uint32_t>       image_indices;

            for (auto window : m_to_present) {
                swapchains.push_back(window->swapchain());
                image_indices.push_back(window->current());
            }

            VkPresentInfoKHR present_info{};
            present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = static_cast<uint32_t>(queue_signal.size());
            present_info.pWaitSemaphores    = queue_signal.data();
            present_info.swapchainCount     = static_cast<uint32_t>(m_to_present.size());
            present_info.pSwapchains        = swapchains.data();
            present_info.pImageIndices      = image_indices.data();
            present_info.pResults           = nullptr;

            vkQueuePresentKHR(m_queues->prs_queue(), &present_info);
        }

        m_to_present.clear();
    }

    void VKDriver::init_functions() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::init_functions");

        auto res = volkInitialize();

        if (res != VK_SUCCESS) {
            WG_LOG_ERROR("failed to initialize volk");
            return;
        }
    }
    void VKDriver::init_instance() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::init_instance");

        auto extensions = pack_strings(m_required_extensions);
        auto layers     = pack_strings(m_required_layers);

        VkDebugUtilsMessengerCreateInfoEXT create_info_ext{};
        create_info_ext.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info_ext.pNext = nullptr;
        create_info_ext.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info_ext.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info_ext.pfnUserCallback = debug_callback;
        create_info_ext.pUserData       = nullptr;

        VkApplicationInfo app_info{};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = m_app_name.c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = m_engine_name.c_str();
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = VULKAN_VERSION;

        VkInstanceCreateInfo inst_create_info{};
        inst_create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        inst_create_info.pApplicationInfo        = &app_info;
        inst_create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        inst_create_info.ppEnabledExtensionNames = extensions.data();
        inst_create_info.enabledLayerCount       = static_cast<uint32_t>(layers.size());
        inst_create_info.ppEnabledLayerNames     = layers.data();
        inst_create_info.pNext                   = m_use_validation ? &create_info_ext : nullptr;

        WG_VK_CHECK(vkCreateInstance(&inst_create_info, nullptr, &m_instance));
        volkLoadInstance(m_instance);

        if (m_use_validation) {
            VKDebug::load_inst_functions(m_instance);
            WG_VK_CHECK(VKDebug::vkCreateDebugUtilsMessengerEXT(m_instance, &create_info_ext, nullptr, &m_debug_messenger));
        }
    }
    void VKDriver::init_physical_device_and_queues(VKWindow& window) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::init_physical_device_and_queues");

        uint32_t device_count;
        WG_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr));

        assert(device_count);
        if (!device_count) {
            WG_LOG_ERROR("no vulkan-compatible device in the system");
            return;
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        std::vector<uint32_t>         devices_fallback;
        std::vector<uint32_t>         devices_integrated;
        std::vector<uint32_t>         devices_discrete;
        WG_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data()));

        for (uint32_t i = 0; i < device_count; i++) {
            auto phys_device = devices[i];

            VkPhysicalDeviceProperties device_props;
            vkGetPhysicalDeviceProperties(phys_device, &device_props);

            // Queus properties
            VKQueues queues(phys_device, window.surface_khr());
            if (!queues.is_complete()) {
                WG_LOG_WARNING("incomplete queues for device " << device_props.deviceName);
                continue;
            }

            // Extensions support
            uint32_t device_ext_count;
            WG_VK_CHECK(vkEnumerateDeviceExtensionProperties(phys_device, nullptr, &device_ext_count, nullptr));

            std::vector<VkExtensionProperties> device_ext(device_ext_count);
            WG_VK_CHECK(vkEnumerateDeviceExtensionProperties(phys_device, nullptr, &device_ext_count, device_ext.data()));

            bool supported = true;
            for (auto& required : m_required_device_extensions) {
                bool found = false;
                for (auto& presented : device_ext) {
                    if (required == presented.extensionName) {
                        found = true;
                        break;
                    }
                }
                supported = supported && found;
            }
            if (!supported) {
                WG_LOG_WARNING("requested extensions not supported for " << device_props.deviceName);
                continue;
            }

            // Swap chain properties
            VKSwapChainSupportInfo support_info;
            window.get_support_info(phys_device, queues.prs_queue_family(), support_info);

            if (!support_info.support_presentation || support_info.present_modes.empty() || support_info.formats.empty()) {
                WG_LOG_WARNING("inadequate swap chain support for " << device_props.deviceName);
                continue;
            }

            // Filter discrete and integrated
            if (device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                devices_discrete.push_back(i);
            else if (device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                devices_integrated.push_back(i);
            else
                devices_fallback.push_back(i);
        }

        // select device if it has discrete, it will overwrite the choice
        if (!devices_fallback.empty()) {
            m_phys_device = devices[devices_fallback[0]];
        }
        if (!devices_integrated.empty()) {
            m_phys_device = devices[devices_integrated[0]];
        }
        if (!devices_discrete.empty()) {
            m_phys_device = devices[devices_discrete[0]];
        }
        if (!m_phys_device) {
            WG_LOG_ERROR("failed to select suitable physical device");
            return;
        }

        // init queues
        m_queues = std::make_unique<VKQueues>(m_phys_device, window.surface_khr());

        // fill extensions info
        uint32_t device_ext_count;
        WG_VK_CHECK(vkEnumerateDeviceExtensionProperties(m_phys_device, nullptr, &device_ext_count, nullptr));
        m_device_extensions.resize(device_ext_count);
        WG_VK_CHECK(vkEnumerateDeviceExtensionProperties(m_phys_device, nullptr, &device_ext_count, m_device_extensions.data()));

        // fill caps info
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(m_phys_device, &device_properties);
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(m_phys_device, &device_features);

        auto& limits                                 = device_properties.limits;
        m_device_caps.max_vertex_attributes          = int(limits.maxVertexInputAttributes);
        m_device_caps.max_texture_array_layers       = int(limits.maxImageArrayLayers);
        m_device_caps.max_texture_3d_size            = int(limits.maxImageDimension3D);
        m_device_caps.max_texture_2d_size            = int(limits.maxImageDimension2D);
        m_device_caps.max_texture_1d_size            = int(limits.maxImageDimension1D);
        m_device_caps.max_shader_uniform_buffers     = int(limits.maxPerStageDescriptorUniformBuffers);
        m_device_caps.max_shader_storage_buffers     = int(limits.maxPerStageDescriptorStorageBuffers);
        m_device_caps.max_shader_sampled_textures    = int(limits.maxPerStageDescriptorSamplers);
        m_device_caps.max_color_attachments          = int(limits.maxColorAttachments);
        m_device_caps.max_framebuffer_width          = int(limits.maxFramebufferWidth);
        m_device_caps.max_framebuffer_height         = int(limits.maxFramebufferHeight);
        m_device_caps.max_anisotropy                 = limits.maxSamplerAnisotropy;
        m_device_caps.support_anisotropy             = device_features.samplerAnisotropy;
        m_device_caps.uniform_block_offset_alignment = static_cast<int>(limits.minUniformBufferOffsetAlignment);

#ifdef WG_DEBUG
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(m_phys_device, &device_props);
        WG_LOG_INFO("device: " << device_props.deviceName << " " << device_props.deviceID);
#endif
    }
    void VKDriver::init_device() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::init_device");

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(m_phys_device, &features);

        std::vector<VkDeviceQueueCreateInfo> queues_create_info;
        float                                queues_priority[1] = {1.0f};

        VkDeviceQueueCreateInfo gfx_queue{};
        gfx_queue.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gfx_queue.queueFamilyIndex = m_queues->gfx_queue_family();
        gfx_queue.queueCount       = 1;
        gfx_queue.pQueuePriorities = queues_priority;
        queues_create_info.push_back(gfx_queue);

        if (m_queues->tsf_queue_family() != m_queues->gfx_queue_family()) {
            VkDeviceQueueCreateInfo tsf_queue{};
            tsf_queue.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            tsf_queue.queueFamilyIndex = m_queues->tsf_queue_family();
            tsf_queue.queueCount       = 1;
            tsf_queue.pQueuePriorities = queues_priority;
            queues_create_info.push_back(tsf_queue);
        }
        if (m_queues->prs_queue_family() != m_queues->tsf_queue_family() && m_queues->prs_queue_family() != m_queues->gfx_queue_family()) {
            VkDeviceQueueCreateInfo prs_queue{};
            prs_queue.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            prs_queue.queueFamilyIndex = m_queues->prs_queue_family();
            prs_queue.queueCount       = 1;
            prs_queue.pQueuePriorities = queues_priority;
            queues_create_info.push_back(prs_queue);
        }

        auto device_extensions = pack_strings(m_required_device_extensions);
        auto layers_extensions = pack_strings(m_required_layers);

        // For MoltenVK we must explicitly request VK_KHR_portability_subset extension
        static const char* VULKAN_KHR_PORTABILITY_SUBSET_EXT_NAME = "VK_KHR_portability_subset";
        if (supports(VULKAN_KHR_PORTABILITY_SUBSET_EXT_NAME)) {
            device_extensions.push_back(VULKAN_KHR_PORTABILITY_SUBSET_EXT_NAME);
        }

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pEnabledFeatures        = &features;
        device_create_info.queueCreateInfoCount    = static_cast<uint32_t>(queues_create_info.size());
        device_create_info.pQueueCreateInfos       = queues_create_info.data();
        device_create_info.enabledExtensionCount   = static_cast<uint32_t>(device_extensions.size());
        device_create_info.ppEnabledExtensionNames = device_extensions.data();
        device_create_info.enabledLayerCount       = static_cast<uint32_t>(layers_extensions.size());
        device_create_info.ppEnabledLayerNames     = layers_extensions.data();

        WG_VK_CHECK(vkCreateDevice(m_phys_device, &device_create_info, nullptr, &m_device));
    }
    void VKDriver::init_pipeline_cache() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::init_pipeline_cache");

        std::vector<uint8_t> cache_data;
        if (!m_file_system->read_file(m_pipeline_cache_path, cache_data)) {
            WG_LOG_INFO("no cache file at " << m_pipeline_cache_path);
            WG_LOG_INFO("creating empty vk pipeline cache");
        } else {
            WG_LOG_INFO("load pipeline cache: " << m_pipeline_cache_path << " " << StringUtils::from_mem_size(cache_data.size()));
        }

        VkPipelineCacheCreateInfo info{};
        info.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        info.initialDataSize = cache_data.size();
        info.pInitialData    = cache_data.data();

        WG_VK_CHECK(vkCreatePipelineCache(m_device, &info, nullptr, &m_pipeline_cache));
        WG_VK_NAME(m_device, m_pipeline_cache, VK_OBJECT_TYPE_PIPELINE_CACHE, m_pipeline_cache_path);
    }
    void VKDriver::release_pipeline_cache() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::release_pipeline_cache");

        if (m_pipeline_cache) {
            size_t cache_size = 0;
            WG_VK_CHECK(vkGetPipelineCacheData(m_device, m_pipeline_cache, &cache_size, nullptr));

            std::vector<uint8_t> cache_data(cache_size);
            WG_VK_CHECK(vkGetPipelineCacheData(m_device, m_pipeline_cache, &cache_size, cache_data.data()));

            vkDestroyPipelineCache(m_device, m_pipeline_cache, nullptr);

            m_file_system->save_file(m_pipeline_cache_path, cache_data);
            WG_LOG_INFO("save pipeline cache: " << m_pipeline_cache_path << " " << StringUtils::from_mem_size(cache_data.size()));
        }
    }
    void VKDriver::init_sync_fences() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::init_sync_fences");

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        WG_VK_CHECK(vkCreateFence(m_device, &fence_info, nullptr, &m_sync_fence));
        WG_VK_NAME(m_device, m_sync_fence, VK_OBJECT_TYPE_FENCE, "sync_fence");
    }
    void VKDriver::release_sync_fences() {
        WG_AUTO_PROFILE_VULKAN("VKDriver::release_sync_fences");

        if (m_sync_fence) {
            vkDestroyFence(m_device, m_sync_fence, nullptr);
        }
    }

    bool VKDriver::supports(const std::string& extension) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::supports");

        for (auto& ext : m_device_extensions) {
            if (extension == ext.extensionName) {
                return true;
            }
        }

        return false;
    }

    void VKDriver::release_resources(uint64_t index) {
        WG_AUTO_PROFILE_VULKAN("VKDriver::release_resources");

        m_deferred_release[index].flush();
    }

    VkBool32 VKDriver::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data) {
        switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                WG_LOG_INFO("validation layer: " << p_callback_data->messageIdNumber << " " << p_callback_data->pMessageIdName << " " << p_callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                WG_LOG_WARNING("validation layer: " << p_callback_data->messageIdNumber << " " << p_callback_data->pMessageIdName << " " << p_callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                WG_LOG_ERROR("validation layer: " << p_callback_data->messageIdNumber << " " << p_callback_data->pMessageIdName << " " << p_callback_data->pMessage);
                break;
            default:
                break;
        }

        return VK_FALSE;
    }

}// namespace wmoge
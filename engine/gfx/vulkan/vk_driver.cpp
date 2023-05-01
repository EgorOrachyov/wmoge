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

#include "core/engine.hpp"
#include "core/string_utils.hpp"
#include "core/task.hpp"
#include "debug/profiler.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_pipeline.hpp"
#include "gfx/vulkan/vk_sampler.hpp"
#include "gfx/vulkan/vk_shader.hpp"
#include "gfx/vulkan/vk_texture.hpp"
#include "platform/file_system.hpp"
#include "resource/config_file.hpp"

#include <cassert>
#include <fstream>

namespace wmoge {

    std::vector<const char*> pack_strings(const std::vector<std::string>& names) {
        std::vector<const char*> names_p(names.size());
        for (int i = 0; i < names.size(); i++) names_p[i] = names[i].c_str();
        return names_p;
    };

    VKDriver::VKDriver(VKInitInfo info) {
        WG_AUTO_PROFILE_VULKAN();

        m_driver_name = SID("vulkan");
        m_clip_matrix = Mat4x4f(1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 0.5f, 0.5f,
                                0.0f, 0.0f, 0.0f, 1.0f);

        m_app_name            = info.app_name;
        m_engine_name         = info.engine_name;
        m_required_extensions = info.required_ext;

        if (m_use_validation) {
            m_required_layers.push_back("VK_LAYER_KHRONOS_validation");
            m_required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        m_required_device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        m_shader_cache_path   = "cache://vk_shader_cache.wgsc";
        m_pipeline_cache_path = "cache://vk_pipeline_cache.wgpc";
        auto* config          = Engine::instance()->config_engine();
        config->get(SID("gfx.vulkan.shader_cache"), m_shader_cache_path);
        config->get(SID("gfx.vulkan.pipeline_cache"), m_pipeline_cache_path);

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
        // init glslang for shader compilation
        init_glslang();
        // init pipeline cache
        init_pipeline_cache();
        // init context required for rendering and commands submission
        init_context();
        // init desc manager
        m_desc_manager = std::make_unique<VKDescManager>(*this);

        WG_LOG_INFO("init vulkan gfx driver");
    }
    VKDriver::~VKDriver() {
        WG_AUTO_PROFILE_VULKAN();

        shutdown();

        WG_LOG_INFO("shutdown vulkan gfx driver");
    }

    ref_ptr<GfxVertFormat> VKDriver::make_vert_format(const GfxVertElements& elements, const StringId& name) {
        std::lock_guard lock(m_cached_mutex);

        auto& format = m_formats[elements];
        if (!format) {
            format = make_ref<VKVertFormat>(elements, name);
            WG_LOG_INFO("cache new format " << name);
        }

        return format;
    }
    ref_ptr<GfxVertBuffer> VKDriver::make_vert_buffer(int size, GfxMemUsage usage, const StringId& name) {
        auto buffer = make_ref<VKVertBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    ref_ptr<GfxIndexBuffer> VKDriver::make_index_buffer(int size, GfxMemUsage usage, const StringId& name) {
        auto buffer = make_ref<VKIndexBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    ref_ptr<GfxUniformBuffer> VKDriver::make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name) {
        auto buffer = make_ref<VKUniformBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    ref_ptr<GfxStorageBuffer> VKDriver::make_storage_buffer(int size, GfxMemUsage usage, const StringId& name) {
        auto buffer = make_ref<VKStorageBuffer>(*this);
        buffer->create(size, usage, name);
        return buffer;
    }
    ref_ptr<GfxShader> VKDriver::make_shader(std::string vertex, std::string fragment, const StringId& name) {
        auto shader = make_ref<VKShader>(*this);
        shader->setup(std::move(vertex), std::move(fragment), name);

        auto compile_shader = make_ref<Task>(name, [shader](TaskContext&) {
            shader->compile_from_source();
            return 0;
        });
        compile_shader->run();

        return shader;
    }
    ref_ptr<GfxShader> VKDriver::make_shader(ref_ptr<Data> code, const StringId& name) {
        auto shader = make_ref<VKShader>(*this);
        shader->setup(std::move(code), name);

        auto compile_shader = make_ref<Task>(SID("vk:compile:shader:" + name.str()), [shader](TaskContext&) {
            shader->compile_from_byte_code();
            return 0;
        });
        compile_shader->run();

        return shader;
    }
    ref_ptr<GfxTexture> VKDriver::make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        auto texture = make_ref<VKTexture>(*this);
        texture->create_2d(width, height, mips, format, usages, mem_usage, name);
        return texture;
    }
    ref_ptr<GfxTexture> VKDriver::make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        auto texture = make_ref<VKTexture>(*this);
        texture->create_2d_array(width, height, mips, slices, format, usages, mem_usage, name);
        return texture;
    }
    ref_ptr<GfxTexture> VKDriver::make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        auto texture = make_ref<VKTexture>(*this);
        texture->create_cube(width, height, mips, format, usages, mem_usage, name);
        return texture;
    }
    ref_ptr<GfxSampler> VKDriver::make_sampler(const GfxSamplerDesc& desc, const StringId& name) {
        std::lock_guard lock(m_cached_mutex);

        auto& sampler = m_samplers[desc];
        if (!sampler) {
            sampler = make_ref<VKSampler>(desc, name, *this);
            sampler->create();

            WG_LOG_INFO("cache new sampler " << name);
        }

        return sampler;
    }
    ref_ptr<GfxRenderPass> VKDriver::make_render_pass(GfxRenderPassType pass_type, const StringId& name) {
        return make_ref<VKRenderPass>(pass_type, name, *this);
    }
    ref_ptr<GfxPipeline> VKDriver::make_pipeline(const GfxPipelineState& state, const StringId& name) {
        std::lock_guard lock(m_cached_mutex);

        auto& pipeline = m_pipelines[state];
        if (!pipeline) {
            pipeline = make_ref<VKPipeline>(state, name, *this);
            WG_LOG_INFO("cache new pipeline " << name);
        }

        return pipeline;
    }
    void VKDriver::update_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKVertBuffer*>(buffer.get())->update(m_cmd, offset, range, data);
        else
            queue()->push([=]() { static_cast<VKVertBuffer*>(buffer.get())->update(m_cmd, offset, range, data); });
    }
    void VKDriver::update_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKIndexBuffer*>(buffer.get())->update(m_cmd, offset, range, data);
        else
            queue()->push([=]() { static_cast<VKIndexBuffer*>(buffer.get())->update(m_cmd, offset, range, data); });
    }
    void VKDriver::update_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKUniformBuffer*>(buffer.get())->update(m_cmd, offset, range, data);
        else
            queue()->push([=]() { static_cast<VKUniformBuffer*>(buffer.get())->update(m_cmd, offset, range, data); });
    }
    void VKDriver::update_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKStorageBuffer*>(buffer.get())->update(m_cmd, offset, range, data);
        else
            queue()->push([=]() { static_cast<VKStorageBuffer*>(buffer.get())->update(m_cmd, offset, range, data); });
    }
    void VKDriver::update_texture_2d(const ref_ptr<GfxTexture>& texture, int mip, Rect2i region, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKTexture*>(texture.get())->update_2d(m_cmd, mip, region, data);
        else
            queue()->push([=]() { static_cast<VKTexture*>(texture.get())->update_2d(m_cmd, mip, region, data); });
    }
    void VKDriver::update_texture_2d_array(const ref_ptr<GfxTexture>& texture, int mip, int slice, Rect2i region, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKTexture*>(texture.get())->update_2d_array(m_cmd, mip, slice, region, data);
        else
            queue()->push([=]() { static_cast<VKTexture*>(texture.get())->update_2d_array(m_cmd, mip, slice, region, data); });
    }
    void VKDriver::update_texture_cube(const ref_ptr<GfxTexture>& texture, int mip, int face, Rect2i region, const ref_ptr<Data>& data) {
        if (use_direct_update())
            static_cast<VKTexture*>(texture.get())->update_cube(m_cmd, mip, face, region, data);
        else
            queue()->push([=]() { static_cast<VKTexture*>(texture.get())->update_cube(m_cmd, mip, face, region, data); });
    }

    void* VKDriver::map_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer) {
        assert(buffer);
        return (static_cast<VKVertBuffer*>(buffer.get()))->map();
    }
    void* VKDriver::map_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer) {
        assert(buffer);
        return (static_cast<VKIndexBuffer*>(buffer.get()))->map();
    }
    void* VKDriver::map_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer) {
        assert(buffer);
        return (static_cast<VKUniformBuffer*>(buffer.get()))->map();
    }
    void* VKDriver::map_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer) {
        assert(buffer);
        return (static_cast<VKStorageBuffer*>(buffer.get()))->map();
    }
    void VKDriver::unmap_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer) {
        assert(buffer);

        auto vk_buffer         = static_cast<VKVertBuffer*>(buffer.get());
        auto vk_staging_buffer = vk_buffer->staging_buffer();

        assert(vk_staging_buffer);

        vk_buffer->unmap();

        if (on_gfx_thread() && cmd()) {
            vk_buffer->VKBuffer::update(m_cmd, vk_staging_buffer);
            vk_buffer->barrier(m_cmd);
        } else {
            queue()->push([buff = ref_ptr<VKVertBuffer>(vk_buffer), vk_staging_buffer, this]() {
                buff->VKBuffer::update(cmd(), vk_staging_buffer);
                buff->barrier(cmd());
            });
        }
    }
    void VKDriver::unmap_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer) {
        assert(buffer);

        auto vk_buffer         = static_cast<VKIndexBuffer*>(buffer.get());
        auto vk_staging_buffer = vk_buffer->staging_buffer();

        assert(vk_staging_buffer);

        vk_buffer->unmap();

        if (on_gfx_thread() && cmd()) {
            vk_buffer->VKBuffer::update(m_cmd, vk_staging_buffer);
            vk_buffer->barrier(m_cmd);
        } else {
            queue()->push([buff = ref_ptr<VKIndexBuffer>(vk_buffer), vk_staging_buffer, this]() {
                buff->VKBuffer::update(cmd(), vk_staging_buffer);
                buff->barrier(cmd());
            });
        }
    }
    void VKDriver::unmap_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer) {
        assert(buffer);

        auto vk_buffer         = static_cast<VKUniformBuffer*>(buffer.get());
        auto vk_staging_buffer = vk_buffer->staging_buffer();

        assert(vk_staging_buffer);

        vk_buffer->unmap();

        if (on_gfx_thread() && cmd()) {
            vk_buffer->VKBuffer::update(m_cmd, vk_staging_buffer);
            vk_buffer->barrier(m_cmd);
        } else {
            queue()->push([buff = ref_ptr<VKUniformBuffer>(vk_buffer), vk_staging_buffer, this]() {
                buff->VKBuffer::update(cmd(), vk_staging_buffer);
                buff->barrier(cmd());
            });
        }
    }
    void VKDriver::unmap_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer) {
        assert(buffer);

        auto vk_buffer         = static_cast<VKStorageBuffer*>(buffer.get());
        auto vk_staging_buffer = vk_buffer->staging_buffer();

        assert(vk_staging_buffer);

        vk_buffer->unmap();

        if (on_gfx_thread() && cmd()) {
            vk_buffer->VKBuffer::update(m_cmd, vk_staging_buffer);
            vk_buffer->barrier(m_cmd);
        } else {
            queue()->push([buff = ref_ptr<VKStorageBuffer>(vk_buffer), vk_staging_buffer, this]() {
                buff->VKBuffer::update(cmd(), vk_staging_buffer);
                buff->barrier(cmd());
            });
        }
    }

    void VKDriver::begin_render_pass(const ref_ptr<GfxRenderPass>& pass) {
        WG_AUTO_PROFILE_VULKAN();
        assert(!m_in_render_pass);
        assert(pass);
        m_current_pass   = pass.cast<VKRenderPass>();
        m_in_render_pass = true;
    }
    void VKDriver::bind_target(const ref_ptr<Window>& window) {
        assert(m_in_render_pass);
        assert(window);
        m_current_pass->bind_target(m_window_manager->get_or_create(window));
        m_target_bound = true;
    }
    void VKDriver::bind_color_target(const ref_ptr<GfxTexture>& texture, int target, int mip, int slice) {
        assert(m_in_render_pass);
        assert(texture);
        m_current_pass->bind_color_target(texture.cast<VKTexture>(), target, mip, slice);
        m_target_bound = true;
    }
    void VKDriver::bind_depth_target(const ref_ptr<GfxTexture>& texture, int mip, int slice) {
        assert(m_in_render_pass);
        assert(texture);
        m_current_pass->bind_depth_target(texture.cast<VKTexture>(), mip, slice);
        m_target_bound = true;
    }
    void VKDriver::viewport(const Rect2i& viewport) {
        assert(m_in_render_pass);
        assert(m_target_bound);
        m_viewport = viewport;
    }
    void VKDriver::clear(int target, const Vec4f& color) {
        assert(m_in_render_pass);
        assert(m_target_bound);
        m_clear_color[target] = color;
        m_current_pass->clear_color(target);
    }
    void VKDriver::clear(float depth, int stencil) {
        assert(m_in_render_pass);
        assert(m_target_bound);
        m_clear_depth   = depth;
        m_clear_stencil = stencil;
        m_current_pass->clear_depth_stencil();
    }
    bool VKDriver::bind_pipeline(const ref_ptr<GfxPipeline>& pipeline) {
        WG_AUTO_PROFILE_VULKAN();
        assert(m_in_render_pass);
        assert(m_target_bound);
        assert(pipeline);

        // Check equal
        if (pipeline.get() == m_current_pipeline.get()) {
            return m_current_pipeline->validate();
        }

        prepare_render_pass();

        // Check compiled
        m_current_pipeline = pipeline.cast<VKPipeline>();
        if (!m_current_pipeline->validate()) {
            return false;
        }

        // Bind shader for descriptor manager
        m_current_shader = m_current_pipeline->state().shader.cast<VKShader>();
        m_desc_manager->bind_shader(m_current_shader);

        vkCmdBindPipeline(m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_current_pipeline->pipeline());
        m_pipeline_bound = true;
        return true;
    }
    void VKDriver::bind_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer, int index, int offset) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);
        m_current_vert_buffers[index]         = buffer.cast<VKVertBuffer>();
        m_current_vert_buffers_offsets[index] = offset;
    }
    void VKDriver::bind_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);
        m_current_index_buffer = buffer.cast<VKIndexBuffer>();
        vkCmdBindIndexBuffer(m_cmd, m_current_index_buffer->buffer(), offset, VKDefs::get_index_type(index_type));
    }
    void VKDriver::bind_texture(const StringId& name, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(texture);
        assert(sampler);
        texture.cast<VKTexture>()->transition_layout(m_cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_desc_manager->bind_texture(name, array_element, texture, sampler);
    }
    void VKDriver::bind_texture(const GfxLocation& location, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(texture);
        assert(sampler);
        texture.cast<VKTexture>()->transition_layout(m_cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_desc_manager->bind_texture(location, array_element, texture, sampler);
    }
    void VKDriver::bind_uniform_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);
        m_desc_manager->bind_uniform_buffer(name, offset, range, buffer);
    }
    void VKDriver::bind_uniform_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);
        m_desc_manager->bind_uniform_buffer(location, offset, range, buffer);
    }
    void VKDriver::bind_storage_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);
        m_desc_manager->bind_storage_buffer(name, offset, range, buffer);
    }
    void VKDriver::bind_storage_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);
        m_desc_manager->bind_storage_buffer(location, offset, range, buffer);
    }
    void VKDriver::draw(int vertex_count, int base_vertex, int instance_count) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        prepare_draw();
        vkCmdDraw(m_cmd, vertex_count, instance_count, base_vertex, 0);
    }
    void VKDriver::draw_indexed(int index_count, int base_vertex, int instance_count) {
        assert(m_pipeline_bound);
        assert(m_target_bound);
        prepare_draw();
        vkCmdDrawIndexed(m_cmd, index_count, instance_count, 0, base_vertex, 0);
    }
    void VKDriver::end_render_pass() {
        WG_AUTO_PROFILE_VULKAN();
        assert(m_in_render_pass);

        if (m_render_pass_started) {
            WG_VK_END_LABEL(m_cmd);
            vkCmdEndRenderPass(m_cmd);
        }

        m_current_pass.reset();
        m_current_pipeline.reset();
        m_current_shader.reset();
        m_current_index_buffer.reset();
        m_current_vert_buffers.fill(nullptr);
        m_current_vert_buffers_offsets.fill(0);
        m_clear_color.fill(Vec4f());
        m_clear_depth   = 1.0f;
        m_clear_stencil = 0;
        m_viewport      = Rect2i(0, 0, 0, 0);
        m_desc_manager->reset();

        m_in_render_pass      = false;
        m_render_pass_started = false;
        m_pipeline_bound      = false;
        m_target_bound        = false;
    }

    void VKDriver::shutdown() {
        WG_AUTO_PROFILE_VULKAN();

        m_shutdown = true;

        if (m_instance) {
            WG_VK_CHECK(vkDeviceWaitIdle(m_device));
            m_callback_queue.clear();
            m_pipelines.clear();
            m_samplers.clear();
            m_formats.clear();
            m_desc_manager.reset();
            m_window_manager.reset();
            release_context();
            release_pipeline_cache();
            glslang::FinalizeProcess();
            m_mem_manager.reset();
            m_queues.reset();
            if (m_device) vkDestroyDevice(m_device, nullptr);
            if (m_debug_messenger) VKDebug::vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
            vkDestroyInstance(m_instance, nullptr);
            m_device      = VK_NULL_HANDLE;
            m_phys_device = VK_NULL_HANDLE;
            m_instance    = VK_NULL_HANDLE;
        }
    }

    void VKDriver::begin_frame() {
        WG_AUTO_PROFILE_VULKAN();

        m_frame_number.fetch_add(1);
        m_index = m_frame_number.load() % GfxLimits::FRAMES_IN_FLIGHT;

        WG_VK_CHECK(vkWaitForFences(m_device, 1, &m_fences[m_index], VK_TRUE, std::numeric_limits<uint64_t>::max()));
        WG_VK_CHECK(vkResetFences(m_device, 1, &m_fences[m_index]));

        m_mem_manager->update();
        m_desc_manager->update();

        release_resources(m_index);

        m_cmd = m_cmds[m_index];
        WG_VK_CHECK(vkResetCommandPool(m_device, m_cmds_pools[m_index], 0));

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;
        WG_VK_CHECK(vkBeginCommandBuffer(m_cmd, &begin_info));
    }
    void VKDriver::flush() {
        WG_AUTO_PROFILE_VULKAN();

        m_callback_queue.flush();
    }
    void VKDriver::end_frame() {
        WG_AUTO_PROFILE_VULKAN();

        int                               wait_count = static_cast<int>(m_to_present.size());
        std::vector<VkSemaphore>          wait_semaphores;
        std::vector<VkPipelineStageFlags> wait_stages(wait_count, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        wait_semaphores.reserve(wait_count);

        for (auto& window : m_to_present) {
            window->color()[window->current()]->transition_layout(m_cmd, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            wait_semaphores.push_back(window->semaphore());
        }
        m_to_present.clear();

        WG_VK_CHECK(vkEndCommandBuffer(m_cmd));

        VkSemaphore signal_semaphores[] = {m_rendering_finished[m_index]};

        VkSubmitInfo submit_info{};
        submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount   = wait_count;
        submit_info.pWaitSemaphores      = wait_semaphores.data();
        submit_info.pWaitDstStageMask    = wait_stages.data();
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &m_cmd;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores    = signal_semaphores;

        WG_VK_CHECK(vkQueueSubmit(m_queues->gfx_queue(), 1, &submit_info, m_fences[m_index]));
    }
    void VKDriver::prepare_window(const ref_ptr<Window>& window) {
        WG_AUTO_PROFILE_VULKAN();

        auto vk_window = m_window_manager->get_or_create(window);
        vk_window->acquire_next();
        vk_window->color()[vk_window->current()]->transition_layout(m_cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        m_to_present.insert(vk_window);
    }
    void VKDriver::swap_buffers(const ref_ptr<Window>& window) {
        WG_AUTO_PROFILE_VULKAN();

        auto vk_window   = m_window_manager->get_or_create(window);
        auto swapchain   = vk_window->swapchain();
        auto image_index = vk_window->current();

        VkPresentInfoKHR present_info{};
        present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = &m_rendering_finished[m_index];
        present_info.pSwapchains        = &swapchain;
        present_info.swapchainCount     = 1;
        present_info.pImageIndices      = &image_index;
        present_info.pResults           = nullptr;

        vkQueuePresentKHR(m_queues->prs_queue(), &present_info);
    }

    void VKDriver::init_functions() {
        auto res = volkInitialize();
        if (res != VK_SUCCESS) {
            WG_LOG_ERROR("failed to initialize volk");
            return;
        }
    }
    void VKDriver::init_instance() {
        WG_AUTO_PROFILE_VULKAN();

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
        WG_AUTO_PROFILE_VULKAN();

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
        m_device_caps.max_vertex_attributes          = limits.maxVertexInputAttributes;
        m_device_caps.max_combined_uniform_blocks    = limits.maxDescriptorSetUniformBuffers;
        m_device_caps.max_texture_array_layers       = limits.maxImageArrayLayers;
        m_device_caps.max_texture_3d_size            = limits.maxImageDimension3D;
        m_device_caps.max_texture_size               = limits.maxImageDimension2D;
        m_device_caps.max_texture_units              = limits.maxDescriptorSetSampledImages;
        m_device_caps.max_color_attachments          = limits.maxColorAttachments;
        m_device_caps.max_framebuffer_width          = limits.maxFramebufferWidth;
        m_device_caps.max_framebuffer_height         = limits.maxFramebufferHeight;
        m_device_caps.uniform_block_offset_alignment = static_cast<int>(limits.minUniformBufferOffsetAlignment);
        m_device_caps.max_anisotropy                 = limits.maxSamplerAnisotropy;
        m_device_caps.support_anisotropy             = device_features.samplerAnisotropy;

#ifdef WG_DEBUG
        VkPhysicalDeviceProperties device_props;
        vkGetPhysicalDeviceProperties(m_phys_device, &device_props);
        WG_LOG_INFO("device: " << device_props.deviceName << " " << device_props.deviceID);
#endif
    }
    void VKDriver::init_device() {
        WG_AUTO_PROFILE_VULKAN();

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
    void VKDriver::init_glslang() {
        WG_AUTO_PROFILE_VULKAN();

        if (!glslang::InitializeProcess()) {
            WG_LOG_ERROR("failed to init glslang");
        }
    }
    void VKDriver::init_pipeline_cache() {
        WG_AUTO_PROFILE_VULKAN();

        auto file_system = Engine::instance()->file_system();

        std::vector<uint8_t> cache_data;
        if (!file_system->read_file(m_pipeline_cache_path, cache_data)) {
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
        WG_VK_NAME(m_device, m_pipeline_cache, VK_OBJECT_TYPE_PIPELINE_CACHE, "cache@" + m_pipeline_cache_path);
    }
    void VKDriver::release_pipeline_cache() {
        WG_AUTO_PROFILE_VULKAN();

        if (m_pipeline_cache) {
            size_t cache_size = 0;
            WG_VK_CHECK(vkGetPipelineCacheData(m_device, m_pipeline_cache, &cache_size, nullptr));

            std::vector<uint8_t> cache_data(cache_size);
            WG_VK_CHECK(vkGetPipelineCacheData(m_device, m_pipeline_cache, &cache_size, cache_data.data()));

            vkDestroyPipelineCache(m_device, m_pipeline_cache, nullptr);

            auto file_system = Engine::instance()->file_system();
            file_system->save_file(m_pipeline_cache_path, cache_data);
            WG_LOG_INFO("save pipeline cache: " << m_pipeline_cache_path << " " << StringUtils::from_mem_size(cache_data.size()));
        }
    }
    void VKDriver::init_context() {
        WG_AUTO_PROFILE_VULKAN();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.queueFamilyIndex = m_queues->gfx_queue_family();

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            WG_VK_CHECK(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_cmds_pools[i]));
            WG_VK_NAME(m_device, m_cmds_pools[i], VK_OBJECT_TYPE_COMMAND_POOL, "pool@" + std::to_string(i));
        }

        VkCommandBufferAllocateInfo info{};
        info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            info.commandPool = m_cmds_pools[i];
            WG_VK_CHECK(vkAllocateCommandBuffers(m_device, &info, &m_cmds[i]));
            WG_VK_NAME(m_device, m_cmds[i], VK_OBJECT_TYPE_COMMAND_BUFFER, "cmd@" + std::to_string(i));
        }

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            WG_VK_CHECK(vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_rendering_finished[i]));
            WG_VK_NAME(m_device, m_rendering_finished[i], VK_OBJECT_TYPE_SEMAPHORE, "rendering_finished@" + std::to_string(i));
            WG_VK_CHECK(vkCreateFence(m_device, &fence_info, nullptr, &m_fences[i]));
            WG_VK_NAME(m_device, m_fences[i], VK_OBJECT_TYPE_FENCE, "fence@" + std::to_string(i));
        }
    }
    void VKDriver::release_context() {
        WG_AUTO_PROFILE_VULKAN();

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            release_resources(i);

            if (m_cmds_pools[i])
                vkDestroyCommandPool(m_device, m_cmds_pools[i], nullptr);
            if (m_rendering_finished[i])
                vkDestroySemaphore(m_device, m_rendering_finished[i], nullptr);
            if (m_fences[i])
                vkDestroyFence(m_device, m_fences[i], nullptr);
        }
    }

    bool VKDriver::supports(const std::string& extension) {
        WG_AUTO_PROFILE_VULKAN();

        for (auto& ext : m_device_extensions) {
            if (extension == ext.extensionName)
                return true;
        }
        return false;
    }
    void VKDriver::prepare_draw() {
        WG_AUTO_PROFILE_VULKAN();

        std::array<VkBuffer, GfxLimits::MAX_VERT_BUFFERS>     vk_vert_buffers{};
        std::array<VkDeviceSize, GfxLimits::MAX_VERT_BUFFERS> vk_vert_buffers_offsets{};

        int count = 0;

        while (count < GfxLimits::MAX_VERT_BUFFERS) {
            if (!m_current_vert_buffers[count]) break;
            vk_vert_buffers[count]         = m_current_vert_buffers[count]->buffer();
            vk_vert_buffers_offsets[count] = m_current_vert_buffers_offsets[count];
            count += 1;
        }

        vkCmdBindVertexBuffers(m_cmd, 0, count, vk_vert_buffers.data(), vk_vert_buffers_offsets.data());

        int                                                   to_bind_count;
        std::array<VkDescriptorSet, GfxLimits::MAX_DESC_SETS> sets = m_desc_manager->sets(to_bind_count);
        if (to_bind_count > 0) {
            vkCmdBindDescriptorSets(m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_current_shader->layout(), 0, to_bind_count, sets.data(), 0, nullptr);
        }
    }
    void VKDriver::prepare_render_pass() {
        WG_AUTO_PROFILE_VULKAN();

        if (!m_render_pass_started) {
            // Potentially recreate make pass or framebuffer
            m_current_pass->validate();

            std::array<VkClearValue, GfxLimits::MAX_COLOR_TARGETS + 1> clear_values{};
            int                                                        clear_value_count = 0;

            for (int i = 0; i < m_current_pass->color_targets_count(); i++) {
                VkClearColorValue value;
                value.float32[0]                        = m_clear_color[i].x();
                value.float32[1]                        = m_clear_color[i].y();
                value.float32[2]                        = m_clear_color[i].z();
                value.float32[3]                        = m_clear_color[i].w();
                clear_values[clear_value_count++].color = value;
            }
            if (m_current_pass->has_depth_stencil()) {
                VkClearDepthStencilValue value;
                value.depth                                    = m_clear_depth;
                value.stencil                                  = m_clear_stencil;
                clear_values[clear_value_count++].depthStencil = value;
            }

            VkRect2D render_area{};
            render_area.offset.x      = 0;
            render_area.offset.y      = 0;
            render_area.extent.width  = m_current_pass->width();
            render_area.extent.height = m_current_pass->height();

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass      = m_current_pass->render_pass()->render_pass();
            render_pass_info.framebuffer     = m_current_pass->framebuffer()->framebuffer();
            render_pass_info.renderArea      = render_area;
            render_pass_info.clearValueCount = clear_value_count;
            render_pass_info.pClearValues    = clear_values.data();

            vkCmdBeginRenderPass(m_cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
            WG_VK_BEGIN_LABEL(m_cmd, m_current_pass->name());

            VkViewport viewport;
            viewport.x        = static_cast<float>(m_viewport.x());
            viewport.y        = static_cast<float>(m_viewport.y());
            viewport.width    = static_cast<float>(m_viewport.z());
            viewport.height   = static_cast<float>(m_viewport.w());
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(m_cmd, 0, 1, &viewport);

            VkRect2D scissor;
            scissor.offset.x      = m_viewport.x();
            scissor.offset.y      = m_viewport.y();
            scissor.extent.width  = m_viewport.z();
            scissor.extent.height = m_viewport.w();
            vkCmdSetScissor(m_cmd, 0, 1, &scissor);

            m_render_pass_started = true;
        }
    }
    void VKDriver::release_resources(uint64_t index) {
        WG_AUTO_PROFILE_VULKAN();
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
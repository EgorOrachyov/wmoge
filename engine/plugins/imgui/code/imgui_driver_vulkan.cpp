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

#include "imgui_driver_vulkan.hpp"

#include "gfx/vulkan/vk_cmd_list.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "gfx/vulkan/vk_render_pass.hpp"
#include "gfx/vulkan/vk_sampler.hpp"
#include "gfx/vulkan/vk_texture.hpp"
#include "gfx/vulkan/vk_window.hpp"

#include "backends/imgui_impl_vulkan.h"

namespace wmoge {

    static void imgui_vulkan_check_error(VkResult result) {
        if (result != VK_SUCCESS) {
            WG_LOG_ERROR("imgui vulkan error " << result);
        }
    }

    ImguiDriverVulkan::ImguiDriverVulkan(const Ref<Window>& window, GfxDriver* driver) {
        GfxRenderPassDesc rp_desc;
        rp_desc.color_target_fmts[0] = driver->get_window_props(window).color_format;
        rp_desc.color_target_ops[0]  = GfxRtOp::LoadStore;

        m_render_pass = driver->make_render_pass(rp_desc, SIDDBG("window_pass"));
        m_window      = window;

        VKDriver*     vk_driver      = static_cast<VKDriver*>(driver);
        Ref<VKWindow> vk_window      = vk_driver->window_manager()->get_or_create(window);
        VKRenderPass* vk_render_pass = static_cast<VKRenderPass*>(m_render_pass.get());

        ImGui_ImplVulkan_InitInfo info;
        info.Instance            = vk_driver->instance();
        info.PhysicalDevice      = vk_driver->phys_device();
        info.Device              = vk_driver->device();
        info.QueueFamily         = vk_driver->queues()->gfx_queue_family();
        info.Queue               = vk_driver->queues()->gfx_queue();
        info.PipelineCache       = vk_driver->pipeline_cache();
        info.DescriptorPool      = VK_NULL_HANDLE;
        info.DescriptorPoolSize  = 1024;
        info.RenderPass          = vk_render_pass->render_pass();
        info.Subpass             = 0;
        info.MinImageCount       = vk_window->min_image_count();
        info.ImageCount          = vk_window->image_count();
        info.MSAASamples         = VK_SAMPLE_COUNT_1_BIT;
        info.Allocator           = nullptr;
        info.CheckVkResultFn     = imgui_vulkan_check_error;
        info.MinAllocationSize   = 1024 * 1024;
        info.UseDynamicRendering = false;

        ImGui_ImplVulkan_Init(&info);
    }

    ImguiDriverVulkan::~ImguiDriverVulkan() {
        textures_clear();
        ImGui_ImplVulkan_Shutdown();
    }

    void ImguiDriverVulkan::new_frame(std::size_t frame_id) {
        m_frame_id = frame_id;
        ImGui_ImplVulkan_NewFrame();
        textures_gc();
    }

    void ImguiDriverVulkan::render(const GfxCmdListRef& cmd_list) {
        ImDrawData* mainDrawData = ImGui::GetDrawData();
        assert(mainDrawData);

        if (mainDrawData->DisplaySize.x > 0.0f && mainDrawData->DisplaySize.y > 0.0f) {
            VKCmdList* vk_cmd_list = static_cast<VKCmdList*>(cmd_list.get());

            GfxRenderPassWindowBeginInfo rp_info;
            rp_info.render_pass = m_render_pass;
            rp_info.window      = m_window;
            rp_info.clear_color = Color::BLACK4f;
            rp_info.name        = SIDDBG("imgui_draw");
            rp_info.area        = Rect2i{0, 0, m_window->fbo_width(), m_window->fbo_height()};

            cmd_list->begin_render_pass(rp_info);
            ImGui_ImplVulkan_RenderDrawData(mainDrawData, vk_cmd_list->get_handle());
            cmd_list->end_render_pass();
        }
    }

    ImTextureID ImguiDriverVulkan::get_texture_id(const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) {
        auto query = m_texture_ids.find(texture);
        if (query != m_texture_ids.end()) {
            query->second.last_frame_used = m_frame_id;
            return query->second.id;
        }

        const VKTexture*      vk_texture = static_cast<VKTexture*>(texture.get());
        const VKSampler*      vk_sampler = static_cast<VKSampler*>(sampler.get());
        const VkDescriptorSet vk_dset    = ImGui_ImplVulkan_AddTexture(vk_sampler->sampler(), vk_texture->view(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        TextureInfo& info    = m_texture_ids[texture];
        info.last_frame_used = m_frame_id;
        info.id              = (ImTextureID) vk_dset;

        return info.id;
    }

    void ImguiDriverVulkan::textures_clear() {
        for (auto& texture_id : m_texture_ids) {
            ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet) texture_id.second.id);
        }
        m_texture_ids.clear();
    }

    void ImguiDriverVulkan::textures_gc() {
        auto iter = m_texture_ids.begin();
        while (iter != m_texture_ids.end()) {
            if (iter->second.last_frame_used + 1 < m_frame_id) {
                ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet) iter->second.id);
                iter = m_texture_ids.erase(iter);
            } else {
                ++iter;
            }
        }
    }

}// namespace wmoge
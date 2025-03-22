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
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_render_pass.hpp"
#include "gfx/vulkan/vk_texture.hpp"

#include <array>
#include <memory>
#include <string>

namespace wmoge {

    /**
     * @class VKSwapChainSupportInfo
     * @brief Info used to select physical device and configure swap chain
     */
    struct VKSwapChainSupportInfo {
        VkSurfaceCapabilitiesKHR        capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   present_modes;
        bool                            support_presentation = false;
    };

    /**
     * @class VKWindow
     * @brief Wrapper for a vulkan surface and platform window presentation
     */
    class VKWindow : public RefCnt {
    public:
        VKWindow(Ref<Window> window, VkSurfaceKHR surface, class VKDriver& driver);
        ~VKWindow() override;

        void                init();
        bool                acquire_next();
        void                get_support_info(VkPhysicalDevice device, uint32_t prs_family, VKSwapChainSupportInfo& info) const;
        Ref<GfxFrameBuffer> get_or_create_frame_buffer(const GfxFrameBufferDesc& desc, const Strid& name);

        [[nodiscard]] const buffered_vector<Ref<VKTexture>>& color() const { return m_color_targets; }
        [[nodiscard]] const Ref<VKTexture>&                  depth_stencil() const { return m_depth_stencil_target; }
        [[nodiscard]] VkSurfaceKHR                           surface_khr() const { return m_surface; }
        [[nodiscard]] VkSwapchainKHR                         swapchain() const { return m_swapchain; }
        [[nodiscard]] VkSemaphore                            acquire_semaphore() const { return m_acquire_semaphore[m_semaphore_index]; }
        [[nodiscard]] VkSemaphore                            present_semaphore() const { return m_present_semaphore[m_semaphore_index]; }

        [[nodiscard]] int      width() const { return int(m_extent.width); }
        [[nodiscard]] int      height() const { return int(m_extent.height); }
        [[nodiscard]] uint32_t current() const { return m_current; }
        [[nodiscard]] uint32_t min_image_count() const { return m_capabilities.minImageCount; }
        [[nodiscard]] uint32_t image_count() const { return m_image_count; }

    private:
        void create_image_semaphores();
        void select_properties();
        void create_swapchain();
        void release_swapchain();
        void recreate_swapchain();
        void check_requested_size();

    private:
        VkSurfaceKHR                    m_surface;
        VkSurfaceFormatKHR              m_surface_format;
        VkSurfaceCapabilitiesKHR        m_capabilities{};
        VkSwapchainKHR                  m_swapchain = VK_NULL_HANDLE;
        VkExtent2D                      m_extent{};
        VkExtent2D                      m_requested_extent{};
        VkPresentModeKHR                m_vsync       = VK_PRESENT_MODE_MAX_ENUM_KHR;
        VkPresentModeKHR                m_performance = VK_PRESENT_MODE_MAX_ENUM_KHR;
        buffered_vector<Ref<VKTexture>> m_color_targets;
        Ref<VKTexture>                  m_depth_stencil_target;

        Ref<Window> m_window;

        std::array<VkSemaphore, GfxLimits::FRAMES_IN_FLIGHT> m_acquire_semaphore{};
        std::array<VkSemaphore, GfxLimits::FRAMES_IN_FLIGHT> m_present_semaphore{};

        flat_map<GfxFrameBufferDesc, Ref<GfxFrameBuffer>> m_frame_buffers;

        uint32_t m_image_count     = 0;
        uint32_t m_current         = 0;
        int      m_semaphore_index = 0;
        bool     m_use_vsync       = true;

        class VKDriver& m_driver;
    };

    /**
     * @class VKWindowManager
     * @brief Manager for vk windows
     */
    class VKWindowManager {
    public:
        VKWindowManager(const VKInitInfo& init_info, class VKDriver& driver);

        [[nodiscard]] Ref<VKWindow> get_or_create(const Ref<Window>& window);
        [[nodiscard]] Ref<VKWindow> get(const Ref<Window>& window);

    private:
        flat_map<Strid, Ref<VKWindow>>                                  m_windows;
        std::function<VkResult(VkInstance, Ref<Window>, VkSurfaceKHR&)> m_factory;
        class VKDriver&                                                 m_driver;
    };

}// namespace wmoge
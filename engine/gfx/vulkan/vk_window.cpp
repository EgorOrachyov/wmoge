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

#include "vk_window.hpp"

#include "event/event_manager.hpp"
#include "event/event_window.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "gfx/vulkan/vk_texture.hpp"
#include "math/math_utils.hpp"
#include "profiler/profiler.hpp"
#include "system/config_file.hpp"
#include "system/engine.hpp"

namespace wmoge {

    VKWindow::VKWindow(Ref<Window> window, VkSurfaceKHR surface, class VKDriver& driver) : m_driver(driver) {
        m_window  = window;
        m_surface = surface;

        Engine::instance()->config()->get(SID("window.vsync"), m_use_vsync);
    }
    VKWindow::~VKWindow() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::~VKWindow");

        WG_VK_CHECK(vkDeviceWaitIdle(m_driver.device()));

        if (m_window_event.is_valid() && Engine::instance()->event_manager()) {
            Engine::instance()->event_manager()->unsubscribe(m_window_event);
        }

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            if (m_acquire_semaphore[i]) {
                vkDestroySemaphore(m_driver.device(), m_acquire_semaphore[i], nullptr);
            }
            if (m_present_semaphore[i]) {
                vkDestroySemaphore(m_driver.device(), m_present_semaphore[i], nullptr);
            }
        }

        release_swapchain();

        vkDestroySurfaceKHR(m_driver.instance(), m_surface, nullptr);
    }
    void VKWindow::init() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::init");

        create_image_semaphores();
        subscribe();
        select_properties();
        create_swapchain();
    }
    void VKWindow::get_support_info(VkPhysicalDevice device, uint32_t prs_family, VKSwapChainSupportInfo& info) const {
        WG_AUTO_PROFILE_VULKAN("VKWindow::get_support_info");

        WG_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &info.capabilities));

        uint32_t format_count = 0;
        WG_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr));
        info.formats.resize(format_count);
        WG_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, info.formats.data()));

        uint32_t present_mode_count = 0;
        WG_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr));
        info.present_modes.resize(present_mode_count);
        WG_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, info.present_modes.data()));

        VkBool32 support = VK_FALSE;
        WG_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, prs_family, m_surface, &support));
        info.support_presentation = support;
    }

    void VKWindow::create_image_semaphores() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::create_image_semaphores");

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            WG_VK_CHECK(vkCreateSemaphore(m_driver.device(), &semaphore_info, nullptr, &m_acquire_semaphore[i]));
            WG_VK_NAME(m_driver.device(), m_acquire_semaphore[i], VK_OBJECT_TYPE_SEMAPHORE, "sem_acquire " + m_window->id().str() + " id=" + std::to_string(i));

            WG_VK_CHECK(vkCreateSemaphore(m_driver.device(), &semaphore_info, nullptr, &m_present_semaphore[i]));
            WG_VK_NAME(m_driver.device(), m_acquire_semaphore[i], VK_OBJECT_TYPE_SEMAPHORE, "sem_present " + m_window->id().str() + " id=" + std::to_string(i));
        }
    }
    void VKWindow::subscribe() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::subscribe");

        m_window_event = Engine::instance()->event_manager()->subscribe<EventWindow>([=](const EventWindow& event) {
            if (event.window == m_window && event.notification == WindowNotification::FramebufferResized) {
                m_requested_extent.width  = m_window->fbo_width();
                m_requested_extent.height = m_window->fbo_height();
            }
            return false;
        });
    }
    void VKWindow::select_properties() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::select_properties");

        VKSwapChainSupportInfo support_info{};
        get_support_info(m_driver.phys_device(), m_driver.queues()->prs_queue_family(), support_info);

        VkFormat desired_formats[] = {VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB};
        for (auto desired : desired_formats) {
            bool found = false;
            for (auto suppoerted : support_info.formats) {
                if (suppoerted.format == desired) {
                    m_surface_format = suppoerted;
                    found            = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }

        bool immediate;
        for (auto mode : support_info.present_modes) {
            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                immediate = true;
            }
        }
        m_performance = immediate ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_FIFO_KHR;
        m_vsync       = VK_PRESENT_MODE_FIFO_KHR;
    }
    void VKWindow::create_swapchain() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::create_swapchain");

        WG_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_driver.phys_device(), m_surface, &m_capabilities))

        static const uint32_t MAX = std::numeric_limits<uint32_t>::max();
        if (m_capabilities.currentExtent.width != MAX ||
            m_capabilities.currentExtent.height != MAX) {
            m_extent = m_capabilities.currentExtent;
        } else {
            uint32_t width      = m_window->fbo_width();
            uint32_t height     = m_window->fbo_height();
            auto     max_extent = m_capabilities.maxImageExtent;
            auto     min_extent = m_capabilities.minImageExtent;
            m_extent.width      = Math::clamp(width, min_extent.width, max_extent.width);
            m_extent.height     = Math::clamp(height, min_extent.height, max_extent.height);
        }

        uint32_t imageCount = 3;
        if (m_capabilities.maxImageCount > 0) {
            imageCount = Math::clamp(imageCount, m_capabilities.minImageCount, m_capabilities.maxImageCount);
        } else {
            imageCount = Math::max(imageCount, m_capabilities.minImageCount);
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface               = m_surface;
        create_info.minImageCount         = imageCount;
        create_info.imageFormat           = m_surface_format.format;
        create_info.imageColorSpace       = m_surface_format.colorSpace;
        create_info.imageExtent           = m_extent;
        create_info.imageArrayLayers      = 1;
        create_info.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        create_info.imageSharingMode      = m_driver.queues()->mode();
        create_info.queueFamilyIndexCount = static_cast<uint32_t>(m_driver.queues()->unique_families().size());
        create_info.pQueueFamilyIndices   = m_driver.queues()->unique_families().data();
        create_info.preTransform          = m_capabilities.currentTransform;
        create_info.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode           = m_use_vsync ? m_vsync : m_performance;
        create_info.clipped               = VK_FALSE;
        create_info.oldSwapchain          = m_swapchain;

        VkSwapchainKHR new_swapchain;

        WG_VK_CHECK(vkCreateSwapchainKHR(m_driver.device(), &create_info, nullptr, &new_swapchain));
        WG_VK_NAME(m_driver.device(), new_swapchain, VK_OBJECT_TYPE_SWAPCHAIN_KHR, m_window->id().str());

        WG_LOG_INFO("create swapchain " << m_window->id() << " " << m_extent.width << "x" << m_extent.height << " images=" << imageCount);

        release_swapchain();
        m_swapchain = new_swapchain;

        uint32_t color_image_count;
        WG_VK_CHECK(vkGetSwapchainImagesKHR(m_driver.device(), m_swapchain, &color_image_count, nullptr));
        std::vector<VkImage> color_images(color_image_count);
        WG_VK_CHECK(vkGetSwapchainImagesKHR(m_driver.device(), m_swapchain, &color_image_count, color_images.data()));

        m_color_targets.resize(color_image_count);

        for (uint32_t i = 0; i < color_image_count; i++) {
            m_color_targets[i] = make_ref<VKTexture>(m_driver);
            m_color_targets[i]->create_2d(m_driver.vk_ctx()->cmd_current(), width(), height(), color_images[i], m_surface_format.format, m_window->id());
        }

        GfxTexUsages depth_stencil_usages;
        depth_stencil_usages.set(GfxTexUsageFlag::DepthStencilTarget);
        m_depth_stencil_target = make_ref<VKTexture>(m_driver);
        m_depth_stencil_target->create_2d(m_driver.vk_ctx()->cmd_current(), width(), height(), 1, GfxFormat::DEPTH24_STENCIL8, depth_stencil_usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, m_window->id());

        m_requested_extent = m_extent;
        m_version += 1;
    }
    void VKWindow::release_swapchain() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::release_swapchain");

        if (m_swapchain) {
            m_color_targets.clear();
            m_depth_stencil_target.reset();

            vkDestroySwapchainKHR(m_driver.device(), m_swapchain, nullptr);
            m_swapchain = nullptr;
        }
    }
    void VKWindow::recreate_swapchain() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::recreate_swapchain");

        // ensure that window resources are no more used
        WG_VK_CHECK(vkDeviceWaitIdle(m_driver.device()));
        // recreate (release called internally)
        create_swapchain();
    }
    void VKWindow::acquire_next() {
        WG_AUTO_PROFILE_VULKAN("VKWindow::acquire_next");

        if (m_requested_extent.width != m_extent.width ||
            m_requested_extent.height != m_extent.height) {
            recreate_swapchain();
        }

        m_semaphore_index = (m_semaphore_index + 1) % GfxLimits::FRAMES_IN_FLIGHT;
        auto semaphore    = m_acquire_semaphore[m_semaphore_index];

        while (true) {
            auto timeout   = std::numeric_limits<uint64_t>::max();
            auto vk_result = vkAcquireNextImageKHR(m_driver.device(), m_swapchain, timeout, semaphore, VK_NULL_HANDLE, &m_current);

            if (vk_result == VK_SUCCESS) {
                break;
            } else if (vk_result == VK_ERROR_OUT_OF_DATE_KHR || vk_result == VK_SUBOPTIMAL_KHR) {
                recreate_swapchain();
                break;
            } else {
                WG_LOG_ERROR("failed to acquired next image");
                return;
            }
        }
    }

    VKWindowManager::VKWindowManager(const VKInitInfo& init_info, class VKDriver& driver) : m_driver(driver) {
        m_factory = init_info.factory;
    }
    Ref<VKWindow> VKWindowManager::get_or_create(const Ref<Window>& window) {
        WG_AUTO_PROFILE_VULKAN("VKWindowManager::get_or_create");

        auto query = m_windows.find(window->id());
        if (query != m_windows.end()) {
            return query->second;
        }

        VkSurfaceKHR surface;
        WG_VK_CHECK(m_factory(m_driver.instance(), window, surface));

        auto vk_window = make_ref<VKWindow>(window, surface, m_driver);
        vk_window->init();

        m_windows[window->id()] = vk_window;
        return vk_window;
    }

}// namespace wmoge

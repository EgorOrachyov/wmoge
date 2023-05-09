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

#include "vk_defs.hpp"
#include "vk_driver.hpp"

#include <cassert>

namespace wmoge {

    PFN_vkCreateDebugUtilsMessengerEXT  VKDebug::vkCreateDebugUtilsMessengerEXT  = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT VKDebug::vkDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkSetDebugUtilsObjectNameEXT    VKDebug::vkSetDebugUtilsObjectNameEXT    = nullptr;
    PFN_vkCmdBeginDebugUtilsLabelEXT    VKDebug::vkCmdBeginDebugUtilsLabelEXT    = nullptr;
    PFN_vkCmdEndDebugUtilsLabelEXT      VKDebug::vkCmdEndDebugUtilsLabelEXT      = nullptr;

    void VKDebug::load_inst_functions(VkInstance instance) {
        vkCreateDebugUtilsMessengerEXT  = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        vkSetDebugUtilsObjectNameEXT    = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
        vkCmdBeginDebugUtilsLabelEXT    = (PFN_vkCmdBeginDebugUtilsLabelEXT) vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
        vkCmdEndDebugUtilsLabelEXT      = (PFN_vkCmdEndDebugUtilsLabelEXT) vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");

        assert(vkCreateDebugUtilsMessengerEXT);
        assert(vkDestroyDebugUtilsMessengerEXT);
        assert(vkSetDebugUtilsObjectNameEXT);
        assert(vkCmdBeginDebugUtilsLabelEXT);
        assert(vkCmdEndDebugUtilsLabelEXT);
    }
    void VKDebug::add_debug_name(VkDevice device, void* object, VkObjectType object_type, const char* name) {
        if (vkSetDebugUtilsObjectNameEXT) {
            VkDebugUtilsObjectNameInfoEXT name_info{};
            name_info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            name_info.pNext        = nullptr;
            name_info.objectType   = object_type;
            name_info.objectHandle = reinterpret_cast<uint64_t>(object);
            name_info.pObjectName  = name;
            WG_VK_CHECK(vkSetDebugUtilsObjectNameEXT(device, &name_info));
        }
    }
    void VKDebug::add_debug_name(VkDevice device, void* object, VkObjectType object_type, const std::string& name) {
        add_debug_name(device, object, object_type, name.c_str());
    }
    void VKDebug::add_debug_name(VkDevice device, void* object, VkObjectType object_type, const StringId& name) {
        add_debug_name(device, object, object_type, name.str().c_str());
    }
    void VKDebug::add_debug_name(VkDevice device, uint64_t object, VkObjectType object_type, const char* name) {
        add_debug_name(device, reinterpret_cast<void*>(object), object_type, name);
    }
    void VKDebug::add_debug_name(VkDevice device, uint64_t object, VkObjectType object_type, const std::string& name) {
        add_debug_name(device, object, object_type, name.c_str());
    }
    void VKDebug::add_debug_name(VkDevice device, uint64_t object, VkObjectType object_type, const StringId& name) {
        add_debug_name(device, object, object_type, name.str().c_str());
    }
    void VKDebug::begin_label(VkCommandBuffer buffer, const char* name, const Vec3f& color) {
        if (vkCmdBeginDebugUtilsLabelEXT) {
            VkDebugUtilsLabelEXT label_ext{};
            label_ext.sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            label_ext.pNext      = nullptr;
            label_ext.pLabelName = name;
            label_ext.color[0]   = color[0];
            label_ext.color[1]   = color[1];
            label_ext.color[2]   = color[2];
            label_ext.color[3]   = 1.0f;
            vkCmdBeginDebugUtilsLabelEXT(buffer, &label_ext);
        }
    }
    void VKDebug::begin_label(VkCommandBuffer buffer, const std::string& name, const Vec3f& color) {
        begin_label(buffer, name.c_str(), color);
    }
    void VKDebug::begin_label(VkCommandBuffer buffer, const StringId& name, const Vec3f& color) {
        begin_label(buffer, name.str().c_str(), color);
    }
    void VKDebug::end_label(VkCommandBuffer buffer) {
        if (vkCmdEndDebugUtilsLabelEXT) {
            vkCmdEndDebugUtilsLabelEXT(buffer);
        }
    }

    void VKDefs::schedule_delete(class VKDriver& driver, class GfxResource* resource) {
        // Note: resources creation/release is allowed only on gfx thread
        // Since it was last reference, we can schedule callback to gfx
        // thread and safely destroy resource calling standard deletion
        driver.release_queue()->push([resource]() { delete resource; });
    }

}// namespace wmoge

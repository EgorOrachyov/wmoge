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

#ifndef VULKAN_VERSION
    #if defined(__APPLE__)
        #define VULKAN_VERSION VK_API_VERSION_1_1
    #else
        #define VULKAN_VERSION VK_API_VERSION_1_2
    #endif
#endif

#include <cinttypes>
#include <functional>
#include <string>
#include <vector>

#include <volk.h>

#define VMA_STATIC_VULKAN_FUNCTIONS  0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

#include "core/log.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"

#include <cassert>

#define VK_LAYER_KHRONOS_VALIDATION "VK_LAYER_KHRONOS_validation"

#define WG_VK_ERROR(result, what) \
    if ((result) != VK_SUCCESS) { WG_LOG_ERROR("function returned error: " #what); }

#define WG_VK_CHECK(function)         \
    do {                              \
        auto result = function;       \
        WG_VK_ERROR(result, function) \
    } while (false);

#ifdef WG_DEBUG
    #define WG_VK_NAME(device, object, type, name) \
        ::wmoge::VKDebug::add_debug_name(device, object, type, name)

    #define WG_VK_BEGIN_LABEL(buffer, name) \
        ::wmoge::VKDebug::begin_label(buffer, name)

    #define WG_VK_END_LABEL(buffer) \
        ::wmoge::VKDebug::end_label(buffer)
#else
    #define WG_VK_NAME(device, object, type, name)
    #define WG_VK_BEGIN_LABEL(buffer, name)
    #define WG_VK_END_LABEL(buffer)
#endif

namespace wmoge {

    /**
     * @class VKInitInfo
     * @brief Info used to init vk driver
     */
    struct VKInitInfo {
        class IocContainer*                                             ioc;
        Ref<Window>                                                     window;
        std::string                                                     app_name;
        std::string                                                     engine_name;
        std::vector<std::string>                                        required_ext;
        std::function<VkResult(VkInstance, Ref<Window>, VkSurfaceKHR&)> factory;
    };

    /**
     * @class VKDebug
     * @brief Extension functions loader for vulkan debugging
     */
    class VKDebug {
    public:
        // Called once when the instance in initialized
        static void load_inst_functions(VkInstance instance);
        static void add_debug_name(VkDevice device, void* object, VkObjectType object_type, const char* name);
        static void add_debug_name(VkDevice device, void* object, VkObjectType object_type, const std::string& name);
        static void add_debug_name(VkDevice device, void* object, VkObjectType object_type, const Strid& name);
        static void add_debug_name(VkDevice device, uint64_t object, VkObjectType object_type, const char* name);
        static void add_debug_name(VkDevice device, uint64_t object, VkObjectType object_type, const std::string& name);
        static void add_debug_name(VkDevice device, uint64_t object, VkObjectType object_type, const Strid& name);
        static void begin_label(VkCommandBuffer buffer, const char* name, const Vec3f& color = Vec3f(1, 1, 1));
        static void begin_label(VkCommandBuffer buffer, const std::string& name, const Vec3f& color = Vec3f(1, 1, 1));
        static void begin_label(VkCommandBuffer buffer, const Strid& name, const Vec3f& color = Vec3f(1, 1, 1));
        static void end_label(VkCommandBuffer buffer);

        static PFN_vkCreateDebugUtilsMessengerEXT  vkCreateDebugUtilsMessengerEXT;
        static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
        static PFN_vkSetDebugUtilsObjectNameEXT    vkSetDebugUtilsObjectNameEXT;
        static PFN_vkCmdBeginDebugUtilsLabelEXT    vkCmdBeginDebugUtilsLabelEXT;
        static PFN_vkCmdEndDebugUtilsLabelEXT      vkCmdEndDebugUtilsLabelEXT;
    };

    class VKDefs {
    public:
        static void schedule_delete(class VKDriver& driver, class GfxResource* resource);

        static VkPipelineStageFlagBits get_query_flag(GfxQueryFlag flag) {
            switch (flag) {
                case GfxQueryFlag::PipelineTop:
                    return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

                case GfxQueryFlag::PipelineBottom:
                    return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

                default:
                    return VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
            }
        }

        static VkShaderStageFlagBits get_shader_module_type(GfxShaderModule module) {
            switch (module) {
                case GfxShaderModule::Vertex:
                    return VK_SHADER_STAGE_VERTEX_BIT;
                case GfxShaderModule::Fragment:
                    return VK_SHADER_STAGE_FRAGMENT_BIT;
                case GfxShaderModule::Compute:
                    return VK_SHADER_STAGE_COMPUTE_BIT;
                default:
                    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            }
        }

        static VkDescriptorType get_desc_type(GfxBindingType type) {
            switch (type) {
                case GfxBindingType::SampledTexture:
                    return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                case GfxBindingType::UniformBuffer:
                    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                case GfxBindingType::StorageBuffer:
                    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                case GfxBindingType::StorageImage:
                    return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                default:
                    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
            }
        }

        static VkImageLayout rt_layout_from_fmt(GfxFormat fmt) {
            switch (fmt) {
                case GfxFormat::DEPTH32F:
                case GfxFormat::DEPTH24_STENCIL8:
                case GfxFormat::DEPTH32F_STENCIL8:
                    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                default:
                    return VK_IMAGE_LAYOUT_UNDEFINED;
            }
        }

        static VkAttachmentLoadOp load_op(GfxRtOp op) {
            switch (op) {
                case GfxRtOp::LoadStore:
                    return VK_ATTACHMENT_LOAD_OP_LOAD;
                case GfxRtOp::ClearStore:
                    return VK_ATTACHMENT_LOAD_OP_CLEAR;
                default:
                    return VK_ATTACHMENT_LOAD_OP_LOAD;
            }
        }

        static VkAttachmentStoreOp store_op(GfxRtOp op) {
            switch (op) {
                case GfxRtOp::LoadStore:
                    return VK_ATTACHMENT_STORE_OP_STORE;
                case GfxRtOp::ClearStore:
                    return VK_ATTACHMENT_STORE_OP_STORE;
                default:
                    return VK_ATTACHMENT_STORE_OP_STORE;
            }
        }

        static bool can_memory_map(GfxMemUsage usage) {
            switch (usage) {
                case GfxMemUsage::CpuVisibleGpu:
                    return true;
                case GfxMemUsage::GpuVisibleCpu:
                    return true;
                case GfxMemUsage::GpuLocal:
                    return false;
                case GfxMemUsage::GpuDedicated:
                    return false;
                default:
                    return false;
            }
        }

        static VkMemoryPropertyFlags get_memory_properties(GfxMemUsage usage) {
            switch (usage) {
                case GfxMemUsage::CpuVisibleGpu:
                    return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                case GfxMemUsage::GpuVisibleCpu:
                    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                case GfxMemUsage::GpuLocal:
                    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                case GfxMemUsage::GpuDedicated:
                    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                default:
                    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            }
        }

        static VmaMemoryUsage get_memory_usage(GfxMemUsage usage) {
            switch (usage) {
                case GfxMemUsage::CpuVisibleGpu:
                    return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
                case GfxMemUsage::GpuVisibleCpu:
                    return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                case GfxMemUsage::GpuLocal:
                    return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                case GfxMemUsage::GpuDedicated:
                    return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                default:
                    return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            }
        }

        static VmaAllocationCreateFlags get_allocation_flags(GfxMemUsage usage) {
            switch (usage) {
                case GfxMemUsage::CpuVisibleGpu:
                    return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                           VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                case GfxMemUsage::GpuVisibleCpu:
                    return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                           VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
                case GfxMemUsage::GpuLocal:
                    return 0;
                case GfxMemUsage::GpuDedicated:
                    return VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
                default:
                    return 0;
            }
        }

        static VkIndexType get_index_type(GfxIndexType type) {
            switch (type) {
                case GfxIndexType::Uint32:
                    return VK_INDEX_TYPE_UINT32;
                case GfxIndexType::Uint16:
                    return VK_INDEX_TYPE_UINT16;
                default:
                    WG_LOG_ERROR("unsupported GfxIndexType");
                    return VK_INDEX_TYPE_MAX_ENUM;
            }
        }

        static VkPrimitiveTopology get_prim_type(GfxPrimType type) {
            switch (type) {
                case GfxPrimType::Triangles:
                    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                case GfxPrimType::Lines:
                    return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                case GfxPrimType::Points:
                    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                default:
                    WG_LOG_ERROR("unsupported GfxPrimType");
                    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
            }
        }

        static VkPolygonMode get_poly_mode(GfxPolyMode mode) {
            switch (mode) {
                case GfxPolyMode::Fill:
                    return VK_POLYGON_MODE_FILL;
                case GfxPolyMode::Line:
                    return VK_POLYGON_MODE_LINE;
                case GfxPolyMode::Point:
                    return VK_POLYGON_MODE_POINT;
                default:
                    WG_LOG_ERROR("unsupported GfxPolyMode");
                    return VK_POLYGON_MODE_MAX_ENUM;
            }
        }

        static VkCullModeFlagBits get_poly_cull_mode(GfxPolyCullMode mode) {
            switch (mode) {
                case GfxPolyCullMode::Disabled:
                    return VK_CULL_MODE_NONE;
                case GfxPolyCullMode::Front:
                    return VK_CULL_MODE_FRONT_BIT;
                case GfxPolyCullMode::Back:
                    return VK_CULL_MODE_BACK_BIT;
                case GfxPolyCullMode::FrontAndBack:
                    return VK_CULL_MODE_FRONT_AND_BACK;
                default:
                    WG_LOG_ERROR("unsupported GfxPolyCullMode");
                    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
            }
        }

        static VkFrontFace get_poly_front_face(GfxPolyFrontFace face) {
            switch (face) {
                case GfxPolyFrontFace::Clockwise:
                    return VK_FRONT_FACE_CLOCKWISE;
                case GfxPolyFrontFace::CounterClockwise:
                    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
                default:
                    WG_LOG_ERROR("unsupported GfxPolyFrontFace");
                    return VK_FRONT_FACE_MAX_ENUM;
            }
        }

        static VkBlendFactor get_blend_fac(GfxBlendFac factor) {
            switch (factor) {
                case GfxBlendFac::Zero:
                    return VK_BLEND_FACTOR_ZERO;
                case GfxBlendFac::One:
                    return VK_BLEND_FACTOR_ONE;
                case GfxBlendFac::SrcColor:
                    return VK_BLEND_FACTOR_SRC_COLOR;
                case GfxBlendFac::OneMinusSrcColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
                case GfxBlendFac::DstColor:
                    return VK_BLEND_FACTOR_DST_COLOR;
                case GfxBlendFac::OneMinusDstColor:
                    return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
                case GfxBlendFac::SrcAlpha:
                    return VK_BLEND_FACTOR_SRC_ALPHA;
                case GfxBlendFac::OneMinusSrcAlpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                case GfxBlendFac::DstAlpha:
                    return VK_BLEND_FACTOR_DST_ALPHA;
                case GfxBlendFac::OneMinusDstAlpha:
                    return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
                default:
                    WG_LOG_ERROR("unsupported GfxBlendFac");
                    return VK_BLEND_FACTOR_MAX_ENUM;
            }
        }

        static VkBlendOp get_blend_op(GfxBlendOp op) {
            switch (op) {
                case GfxBlendOp::Add:
                    return VK_BLEND_OP_ADD;
                case GfxBlendOp::Subtract:
                    return VK_BLEND_OP_SUBTRACT;
                case GfxBlendOp::ReverseSubtract:
                    return VK_BLEND_OP_REVERSE_SUBTRACT;
                case GfxBlendOp::Min:
                    return VK_BLEND_OP_MIN;
                case GfxBlendOp::Max:
                    return VK_BLEND_OP_MAX;
                default:
                    WG_LOG_ERROR("unsupported GfxBlendOp");
                    return VK_BLEND_OP_MAX;
            }
        }

        static bool is_suitable_for_ds(GfxFormat format, bool& depth, bool& stencil) {
            switch (format) {
                case GfxFormat::DEPTH32F:
                    depth   = true;
                    stencil = false;
                    return true;
                case GfxFormat::DEPTH32F_STENCIL8:
                case GfxFormat::DEPTH24_STENCIL8:
                    depth   = true;
                    stencil = true;
                    return true;
                default:
                    depth   = false;
                    stencil = false;
                    return false;
            }
        }

        static VkImageType get_image_type(GfxTex type) {
            switch (type) {
                case GfxTex::Tex2d:
                case GfxTex::Tex2dArray:
                case GfxTex::TexCube:
                    return VK_IMAGE_TYPE_2D;
                default:
                    WG_LOG_ERROR("unsupported GfxTex");
                    return VK_IMAGE_TYPE_MAX_ENUM;
            }
        }

        static VkImageViewType get_view_type(GfxTex type) {
            switch (type) {
                case GfxTex::Tex2d:
                    return VK_IMAGE_VIEW_TYPE_2D;
                case GfxTex::Tex2dArray:
                    return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                case GfxTex::TexCube:
                    return VK_IMAGE_VIEW_TYPE_CUBE;
                default:
                    WG_LOG_ERROR("unsupported GfxTex");
                    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
            }
        }

        static VkImageAspectFlags get_aspect_flags(GfxFormat format) {
            switch (format) {
                case GfxFormat::R8:
                case GfxFormat::R8_SNORM:
                case GfxFormat::R16:
                case GfxFormat::R16_SNORM:
                case GfxFormat::RG8:
                case GfxFormat::RG8_SNORM:
                case GfxFormat::RG16:
                case GfxFormat::RG16_SNORM:
                case GfxFormat::RGB8:
                case GfxFormat::RGB8_SNORM:
                case GfxFormat::RGB16_SNORM:
                case GfxFormat::RGBA8:
                case GfxFormat::RGBA8_SNORM:
                case GfxFormat::RGBA16:
                case GfxFormat::SRGB8:
                case GfxFormat::SRGB8_ALPHA8:
                case GfxFormat::SBGR8_ALPHA8:
                case GfxFormat::R16F:
                case GfxFormat::RG16F:
                case GfxFormat::RGB16F:
                case GfxFormat::RGBA16F:
                case GfxFormat::R32F:
                case GfxFormat::RG32F:
                case GfxFormat::RGB32F:
                case GfxFormat::RGBA32F:
                case GfxFormat::R32I:
                case GfxFormat::RG32I:
                case GfxFormat::RGB32I:
                case GfxFormat::RGBA32I:
                case GfxFormat::BC1_RGB:
                case GfxFormat::BC1_RGB_SRGB:
                case GfxFormat::BC1_RGBA:
                case GfxFormat::BC1_RGBA_SRGB:
                case GfxFormat::BC2:
                case GfxFormat::BC2_SRGB:
                case GfxFormat::BC3:
                case GfxFormat::BC3_SRGB:
                case GfxFormat::BC4:
                case GfxFormat::BC4_SNORM:
                case GfxFormat::BC5:
                case GfxFormat::BC5_SNORM:
                case GfxFormat::BC6H_UFLOAT:
                case GfxFormat::BC6H_SFLOAT:
                case GfxFormat::BC7:
                case GfxFormat::BC7_SRGB:
                    return VK_IMAGE_ASPECT_COLOR_BIT;
                case GfxFormat::DEPTH32F:
                    return VK_IMAGE_ASPECT_DEPTH_BIT;
                case GfxFormat::DEPTH32F_STENCIL8:
                case GfxFormat::DEPTH24_STENCIL8:
                    return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                default:
                    WG_LOG_ERROR("unsupported GfxTexFormat");
                    return VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
            }
        }

        static VkFormat get_format(GfxFormat format) {
            switch (format) {
                case GfxFormat::R8:
                    return VK_FORMAT_R8_UNORM;
                case GfxFormat::R8_SNORM:
                    return VK_FORMAT_R8_SNORM;
                case GfxFormat::R16:
                    return VK_FORMAT_R16_UNORM;
                case GfxFormat::R16_SNORM:
                    return VK_FORMAT_R16_SNORM;
                case GfxFormat::RG8:
                    return VK_FORMAT_R8G8_UNORM;
                case GfxFormat::RG8_SNORM:
                    return VK_FORMAT_R8G8_SNORM;
                case GfxFormat::RG16:
                    return VK_FORMAT_R16G16_UNORM;
                case GfxFormat::RG16_SNORM:
                    return VK_FORMAT_R16G16_SNORM;
                case GfxFormat::RGB8:
                    return VK_FORMAT_R8G8B8_UNORM;
                case GfxFormat::RGB8_SNORM:
                    return VK_FORMAT_R8G8B8_SNORM;
                case GfxFormat::RGB16_SNORM:
                    return VK_FORMAT_R16G16B16_SNORM;
                case GfxFormat::RGBA8:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                case GfxFormat::RGBA8_SNORM:
                    return VK_FORMAT_R8G8B8A8_SNORM;
                case GfxFormat::RGBA16:
                    return VK_FORMAT_R16G16B16A16_UNORM;
                case GfxFormat::SRGB8:
                    return VK_FORMAT_R8G8B8_SRGB;
                case GfxFormat::SRGB8_ALPHA8:
                    return VK_FORMAT_R8G8B8A8_SRGB;
                case GfxFormat::SBGR8_ALPHA8:
                    return VK_FORMAT_B8G8R8A8_SRGB;

                case GfxFormat::R32I:
                    return VK_FORMAT_R32_SINT;
                case GfxFormat::RG32I:
                    return VK_FORMAT_R32G32_SINT;
                case GfxFormat::RGB32I:
                    return VK_FORMAT_R32G32B32_SINT;
                case GfxFormat::RGBA32I:
                    return VK_FORMAT_R32G32B32A32_SINT;

                case GfxFormat::R16F:
                    return VK_FORMAT_R16_SFLOAT;
                case GfxFormat::RG16F:
                    return VK_FORMAT_R16G16_SFLOAT;
                case GfxFormat::RGB16F:
                    return VK_FORMAT_R16G16B16_SFLOAT;
                case GfxFormat::RGBA16F:
                    return VK_FORMAT_R16G16B16A16_SFLOAT;
                case GfxFormat::R32F:
                    return VK_FORMAT_R32_SFLOAT;
                case GfxFormat::RG32F:
                    return VK_FORMAT_R32G32_SFLOAT;
                case GfxFormat::RGB32F:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case GfxFormat::RGBA32F:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;

                case GfxFormat::BC1_RGB:
                    return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
                case GfxFormat::BC1_RGB_SRGB:
                    return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
                case GfxFormat::BC1_RGBA:
                    return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
                case GfxFormat::BC1_RGBA_SRGB:
                    return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
                case GfxFormat::BC2:
                    return VK_FORMAT_BC2_UNORM_BLOCK;
                case GfxFormat::BC2_SRGB:
                    return VK_FORMAT_BC2_SRGB_BLOCK;
                case GfxFormat::BC3:
                    return VK_FORMAT_BC3_UNORM_BLOCK;
                case GfxFormat::BC3_SRGB:
                    return VK_FORMAT_BC3_SRGB_BLOCK;
                case GfxFormat::BC4:
                    return VK_FORMAT_BC4_UNORM_BLOCK;
                case GfxFormat::BC4_SNORM:
                    return VK_FORMAT_BC4_SNORM_BLOCK;
                case GfxFormat::BC5:
                    return VK_FORMAT_BC5_UNORM_BLOCK;
                case GfxFormat::BC5_SNORM:
                    return VK_FORMAT_BC5_SNORM_BLOCK;
                case GfxFormat::BC6H_UFLOAT:
                    return VK_FORMAT_BC6H_UFLOAT_BLOCK;
                case GfxFormat::BC6H_SFLOAT:
                    return VK_FORMAT_BC6H_SFLOAT_BLOCK;
                case GfxFormat::BC7:
                    return VK_FORMAT_BC7_UNORM_BLOCK;
                case GfxFormat::BC7_SRGB:
                    return VK_FORMAT_BC7_SRGB_BLOCK;

                case GfxFormat::DEPTH32F:
                    return VK_FORMAT_D32_SFLOAT;
                case GfxFormat::DEPTH32F_STENCIL8:
                    return VK_FORMAT_D32_SFLOAT_S8_UINT;
                case GfxFormat::DEPTH24_STENCIL8:
                    return VK_FORMAT_D24_UNORM_S8_UINT;
                default:
                    WG_LOG_ERROR("unsupported GfxTexFormat");
                    return VK_FORMAT_MAX_ENUM;
            }
        }

        static VkCompareOp get_comp_func(GfxCompFunc function) {
            switch (function) {
                case GfxCompFunc::Never:
                    return VK_COMPARE_OP_NEVER;
                case GfxCompFunc::Less:
                    return VK_COMPARE_OP_LESS;
                case GfxCompFunc::Equal:
                    return VK_COMPARE_OP_EQUAL;
                case GfxCompFunc::LessEqual:
                    return VK_COMPARE_OP_LESS_OR_EQUAL;
                case GfxCompFunc::Greater:
                    return VK_COMPARE_OP_GREATER;
                case GfxCompFunc::GreaterEqual:
                    return VK_COMPARE_OP_GREATER_OR_EQUAL;
                case GfxCompFunc::NotEqual:
                    return VK_COMPARE_OP_NOT_EQUAL;
                case GfxCompFunc::Always:
                    return VK_COMPARE_OP_ALWAYS;
                default:
                    WG_LOG_ERROR("unsupported GfxCompFunc");
                    return VK_COMPARE_OP_MAX_ENUM;
            }
        }

        static VkStencilOp get_stencil_op(GfxOp operation) {
            switch (operation) {
                case GfxOp::Keep:
                    return VK_STENCIL_OP_KEEP;
                case GfxOp::Zero:
                    return VK_STENCIL_OP_ZERO;
                case GfxOp::Replace:
                    return VK_STENCIL_OP_REPLACE;
                case GfxOp::Increment:
                    return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
                case GfxOp::Decrement:
                    return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
                case GfxOp::Invert:
                    return VK_STENCIL_OP_INVERT;
                default:
                    WG_LOG_ERROR("unsupported GfxOp");
                    return VK_STENCIL_OP_MAX_ENUM;
            }
        }

        static VkSamplerAddressMode get_address_mode(GfxSampAddress address) {
            switch (address) {
                case GfxSampAddress::Repeat:
                    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
                case GfxSampAddress::MirroredRepeat:
                    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                case GfxSampAddress::ClampToEdge:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                case GfxSampAddress::ClampToBorder:
                    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                case GfxSampAddress::MirrorClamToEdge:
                    return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
                default:
                    WG_LOG_ERROR("unsupported GfxSampAddress");
                    return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
            }
        }

        static VkBorderColor get_border_color(GfxSampBrdClr clr) {
            switch (clr) {
                case GfxSampBrdClr::Black:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
                case GfxSampBrdClr::White:
                    return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
                default:
                    WG_LOG_ERROR("unsupported GfxSampBrdClr");
                    return VK_BORDER_COLOR_MAX_ENUM;
            }
        }

        static VkSamplerMipmapMode get_mipmap_mode(GfxSampFlt flt) {
            switch (flt) {
                case GfxSampFlt::Nearest:
                case GfxSampFlt::Linear:
                case GfxSampFlt::NearestMipmapNearest:
                case GfxSampFlt::LinearMipmapNearest:
                    return VK_SAMPLER_MIPMAP_MODE_NEAREST;
                case GfxSampFlt::NearestMipmapLinear:
                case GfxSampFlt::LinearMipmapLinear:
                    return VK_SAMPLER_MIPMAP_MODE_LINEAR;
                default:
                    WG_LOG_ERROR("unsupported GfxSampFlt");
                    return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
            }
        }

        static VkFilter get_filter(GfxSampFlt flt) {
            switch (flt) {
                case GfxSampFlt::Nearest:
                case GfxSampFlt::NearestMipmapNearest:
                case GfxSampFlt::NearestMipmapLinear:
                    return VK_FILTER_NEAREST;
                case GfxSampFlt::Linear:
                case GfxSampFlt::LinearMipmapNearest:
                case GfxSampFlt::LinearMipmapLinear:
                    return VK_FILTER_LINEAR;
                default:
                    WG_LOG_ERROR("unsupported GfxSampFlt");
                    return VK_FILTER_MAX_ENUM;
            }
        }
    };

}// namespace wmoge
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

#include "shader_param_block.hpp"

#include "core/log.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/shader_manager.hpp"
#include "grc/shader_param.hpp"

#include <array>
#include <cassert>
#include <cinttypes>
#include <cstring>
#include <type_traits>

namespace wmoge {

    bool ShaderParamBlockDesc::is_compatible(const ShaderParamBlockDesc& other) const {
        return shader == other.shader && space_idx == other.space_idx;
    }

    struct ShaderParamAccess {
        ShaderParamAccess(ShaderParamBlock& provider) : provider(provider) {}

        template<typename T>
        Status set(ShaderParamId param_id, const T& v) {
            if (param_id.is_invalid()) {
                WG_LOG_ERROR("passed invalid param id");
                return StatusCode::InvalidParameter;
            }

            std::optional<ShaderParamInfo*> p_info = provider.find_param(param_id);

            if (!p_info) {
                WG_LOG_ERROR("no such param id");
                return StatusCode::InvalidParameter;
            }

            ShaderParamInfo* param  = p_info.value();
            Ref<Data>*       buffer = nullptr;

            if (param->buffer != -1) {
                buffer = provider.get_buffer(param->space, param->buffer);
                if (!buffer) {
                    WG_LOG_ERROR("no such buffer");
                    return StatusCode::InvalidState;
                }

                auto param_type = provider.get_shader()->find_type(param->type);
                if (!param_type) {
                    WG_LOG_ERROR("no such type " << param->type.name);
                    return StatusCode::InvalidState;
                }

                const std::int16_t offset  = param->offset;
                const std::int16_t size    = param->elem_count * (*param_type)->byte_size;
                std::uint8_t*      ptr     = (*buffer)->buffer();
                T                  to_copy = v;

                if (size != sizeof(T)) {
                    WG_LOG_ERROR("mismatched param size to set");
                    return StatusCode::InvalidParameter;
                }

                if constexpr (std::is_same_v<T, Mat2x2f>) {
                    to_copy = to_copy.transpose();
                }
                if constexpr (std::is_same_v<T, Mat3x3f>) {
                    to_copy = to_copy.transpose();
                }
                if constexpr (std::is_same_v<T, Mat4x4f>) {
                    to_copy = to_copy.transpose();
                }

                std::memcpy(ptr + offset, (const std::uint8_t*) &to_copy, size);
                return WG_OK;
            }

            GfxDescSetResources* resources = provider.get_gfx_resources(param->space);
            if (!resources) {
                WG_LOG_ERROR("no such resources set");
                return StatusCode::InvalidState;
            }

            GfxDescSetResource& resource   = resources->at(param->binding);
            GfxDescBindValue&   bind_value = resource.second;

            if constexpr (std::is_same_v<T, Ref<GfxUniformBuffer>>) {
                bind_value.resource = v.template as<GfxResource>();
                return WG_OK;
            }
            if constexpr (std::is_same_v<T, Ref<GfxStorageBuffer>>) {
                bind_value.resource = v.template as<GfxResource>();
                return WG_OK;
            }
            if constexpr (std::is_same_v<T, Ref<GfxTexture>>) {
                bind_value.resource = v.template as<GfxResource>();
                return WG_OK;
            }
            if constexpr (std::is_same_v<T, Ref<GfxSampler>>) {
                bind_value.sampler = v;
                return WG_OK;
            }

            WG_LOG_ERROR("unsupported type of param");
            return StatusCode::Error;
        }

        template<typename T>
        Status get(ShaderParamId param_id, T& v) {
            if (param_id.is_invalid()) {
                WG_LOG_ERROR("passed invalid param id");
                return StatusCode::InvalidParameter;
            }

            std::optional<ShaderParamInfo*> p_info = provider.find_param(param_id);

            if (!p_info) {
                WG_LOG_ERROR("no such param id");
                return StatusCode::InvalidParameter;
            }

            ShaderParamInfo* param  = p_info.value();
            Ref<Data>*       buffer = nullptr;

            if (param->buffer != -1) {
                buffer = provider.get_buffer(param->space, param->buffer);
                if (!buffer) {
                    WG_LOG_ERROR("no such buffer");
                    return StatusCode::InvalidState;
                }

                auto param_type = provider.get_shader()->find_type(param->type);
                if (!param_type) {
                    WG_LOG_ERROR("no such type " << param->type.name);
                    return StatusCode::InvalidState;
                }

                const std::int16_t  offset = param->offset;
                const std::int16_t  size   = param->elem_count * (*param_type)->byte_size;
                const std::uint8_t* ptr    = (*buffer)->buffer();
                T                   to_copy;

                if (size != sizeof(T)) {
                    WG_LOG_ERROR("mismatched param size to set");
                    return StatusCode::InvalidParameter;
                }

                std::memcpy((std::uint16_t*) &to_copy, ptr + offset, size);

                if constexpr (std::is_same_v<T, Mat2x2f>) {
                    to_copy = to_copy.transpose();
                }
                if constexpr (std::is_same_v<T, Mat3x3f>) {
                    to_copy = to_copy.transpose();
                }
                if constexpr (std::is_same_v<T, Mat4x4f>) {
                    to_copy = to_copy.transpose();
                }

                v = to_copy;
                return WG_OK;
            }

            const GfxDescSetResources* resources = provider.get_gfx_resources(param->space);
            if (!resources) {
                WG_LOG_ERROR("no such resources set");
                return StatusCode::InvalidState;
            }

            const GfxDescSetResource& resource   = resources->at(param->binding);
            const GfxDescBindValue&   bind_value = resource.second;

            if constexpr (std::is_same_v<T, Ref<GfxUniformBuffer>>) {
                v = bind_value.resource.template cast<GfxUniformBuffer>();
                return WG_OK;
            }
            if constexpr (std::is_same_v<T, Ref<GfxStorageBuffer>>) {
                v = bind_value.resource.template cast<GfxStorageBuffer>();
                return WG_OK;
            }
            if constexpr (std::is_same_v<T, Ref<GfxTexture>>) {
                v = bind_value.resource.template cast<GfxTexture>();
                return WG_OK;
            }
            if constexpr (std::is_same_v<T, Ref<GfxSampler>>) {
                v = bind_value.sampler;
                return WG_OK;
            }

            WG_LOG_ERROR("unsupported type of param");
            return StatusCode::Error;
        }

        ShaderParamBlock& provider;
    };

#define WG_GRC_SET_VAR_BUFF                                     \
    const Status s = ShaderParamAccess(*this).set(param_id, v); \
    if (s) { dirty_buffers(); }                                 \
    return s

#define WG_GRC_SET_VAR_BIND                                     \
    const Status s = ShaderParamAccess(*this).set(param_id, v); \
    if (s) { dirty_set(); }                                     \
    return s

#define WG_GRC_GET_VAR_BUFF                                     \
    const Status s = ShaderParamAccess(*this).get(param_id, v); \
    return s

#define WG_GRC_GET_VAR_BIND                                     \
    const Status s = ShaderParamAccess(*this).get(param_id, v); \
    return s

    ShaderParamBlock::ShaderParamBlock(Shader& shader, std::int16_t space_idx, const Strid& name) {
        init(shader, space_idx, name);
    }

    Status ShaderParamBlock::init(Shader& shader, std::int16_t space_idx, const Strid& name) {
        m_shader = &shader;
        m_space  = space_idx;
        m_name   = name;

        assert(space_idx < m_shader->get_reflection().spaces.size());

        return restore_defaults();
    }

    Status ShaderParamBlock::restore_defaults() {
        if (!m_shader) {
            WG_LOG_ERROR("param block not configured");
            return StatusCode::InvalidState;
        }

        const ShaderSpace& space = m_shader->get_reflection().spaces[m_space];

        if (m_gfx_resources.empty()) {
            m_gfx_resources.resize(space.bindings.size());
        }

        for (std::int16_t i = 0; i < std::int16_t(space.bindings.size()); i++) {
            const ShaderBinding& binding = space.bindings[i];
            GfxDescBindPoint&    p       = m_gfx_resources[i].first;
            GfxDescBindValue&    v       = m_gfx_resources[i].second;

            p.binding       = i;
            p.array_element = 0;
            v.offset        = 0;

            const ShaderTypeIdx    type_idx = binding.type;
            const Ref<ShaderType>& type     = m_shader->get_reflection().type_map[type_idx.idx];

            switch (binding.binding) {
                case ShaderBindingType::InlineUniformBuffer:
                case ShaderBindingType::UniformBuffer:
                    p.type  = GfxBindingType::UniformBuffer;
                    v.range = type->byte_size;
                    break;
                case ShaderBindingType::StorageBuffer:
                    p.type  = GfxBindingType::StorageBuffer;
                    v.range = type->byte_size;
                    break;
                case ShaderBindingType::Sampler2d:
                case ShaderBindingType::Sampler2dArray:
                case ShaderBindingType::SamplerCube:
                    p.type = GfxBindingType::SampledTexture;
                    break;
                case ShaderBindingType::StorageImage2d:
                    p.type = GfxBindingType::StorageImage;
                    break;
                default:
                    break;
            }
        }

        if (m_buffers.empty()) {
            for (const auto& buffer : m_shader->get_reflection().buffers) {
                if (buffer.space == m_space) {
                    m_buffers.emplace_back(make_ref<Data>(buffer.size));
                }
            }
        }

        for (const auto& buffer : m_shader->get_reflection().buffers) {
            if (buffer.space == m_space) {
                const Ref<Data>& src = buffer.defaults;
                const Ref<Data>& dst = m_buffers[buffer.idx];

                assert(src);
                assert(dst);
                assert(src->size() == dst->size());

                std::memcpy(dst->buffer(), src->buffer(), src->size());
            }
        }

        m_gfx_set.reset();

        dirty_buffers();
        dirty_set();

        return WG_OK;
    }

    Status ShaderParamBlock::validate_buffers(GfxDriver* driver, GfxCmdList* cmd_list, buffered_vector<GfxBuffer*>& barrier_buffers) {
        if (!m_shader) {
            WG_LOG_ERROR("param block not configured");
            return StatusCode::InvalidState;
        }
        if (!m_dirty_buffers) {
            return WG_OK;
        }

        const ShaderSpace& space = m_shader->get_reflection().spaces[m_space];

        for (const auto& buffer : m_shader->get_reflection().buffers) {
            if (buffer.space == m_space) {
                const Ref<Data>&  src = m_buffers[buffer.idx];
                GfxDescBindValue& v   = m_gfx_resources[buffer.binding].second;

                if (!v.resource) {
                    v.resource = driver->make_uniform_buffer(buffer.size, GfxMemUsage::GpuLocal, space.bindings[buffer.binding].name);
                    dirty_set();
                }

                Ref<GfxUniformBuffer> buffer = v.resource.cast<GfxUniformBuffer>();
                assert(buffer);
                assert(buffer->size() == src->size());

                cmd_list->update_uniform_buffer(buffer, v.offset, v.range, {src->buffer(), src->size()});
                barrier_buffers.push_back(buffer);
            }
        }

        m_dirty_buffers = 0;
        return WG_OK;
    }

    Status ShaderParamBlock::validate_set(GfxDriver* driver, const GfxDescSetLayoutRef& layout) {
        if (!m_shader) {
            WG_LOG_ERROR("param block not configured");
            return StatusCode::InvalidState;
        }
        if (!m_dirty_set) {
            return WG_OK;
        }

        for (std::int16_t i = 0; i < std::int16_t(m_gfx_resources.size()); i++) {
            const GfxDescBindPoint& p = m_gfx_resources[i].first;
            const GfxDescBindValue& v = m_gfx_resources[i].second;

            if (!v.resource) {
                WG_LOG_ERROR("missing res setup of "
                             << m_name
                             << " space=" << m_space
                             << " binding=" << i
                             << " shader=" << m_shader->get_shader_name());
                return StatusCode::InvalidState;
            }
            if (p.type == GfxBindingType::SampledTexture && !v.sampler) {
                WG_LOG_ERROR("missing sampler setup of "
                             << m_name
                             << " space=" << m_space
                             << " binding=" << i
                             << " shader=" << m_shader->get_shader_name());
                return StatusCode::InvalidState;
            }
        }

        m_gfx_set   = driver->make_desc_set(m_gfx_resources, layout, m_name);
        m_dirty_set = 0;
        return WG_OK;
    }

    Status ShaderParamBlock::set_var(ShaderParamId param_id, int v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, float v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Vec2f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Vec3f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Vec4f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Vec2i& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Vec3i& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Vec4i& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Mat4x4f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Ref<GfxTexture>& v) { WG_GRC_SET_VAR_BIND; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Ref<GfxSampler>& v) { WG_GRC_SET_VAR_BIND; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Ref<GfxUniformBuffer>& v) { WG_GRC_SET_VAR_BIND; }
    Status ShaderParamBlock::set_var(ShaderParamId param_id, const Ref<GfxStorageBuffer>& v) { WG_GRC_SET_VAR_BIND; }

    Status ShaderParamBlock::get_var(ShaderParamId param_id, int& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, float& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Vec2f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Vec3f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Vec4f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Vec2i& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Vec3i& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Vec4i& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Mat4x4f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Ref<GfxTexture>& v) { WG_GRC_GET_VAR_BIND; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Ref<GfxSampler>& v) { WG_GRC_GET_VAR_BIND; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Ref<GfxUniformBuffer>& v) { WG_GRC_GET_VAR_BIND; }
    Status ShaderParamBlock::get_var(ShaderParamId param_id, Ref<GfxStorageBuffer>& v) { WG_GRC_GET_VAR_BIND; }

    Ref<Data>* ShaderParamBlock::get_buffer(std::int16_t buffer_idx) {
        return &m_buffers[buffer_idx];
    }
    Ref<Data>* ShaderParamBlock::get_buffer(std::int16_t space_idx, std::int16_t buffer_idx) {
        if (space_idx != m_space) {
            return nullptr;
        }
        return &m_buffers[buffer_idx];
    }
    GfxDescSetResources* ShaderParamBlock::get_gfx_resources() {
        return &m_gfx_resources;
    }
    GfxDescSetResources* ShaderParamBlock::get_gfx_resources(std::int16_t space_idx) {
        if (space_idx != m_space) {
            return nullptr;
        }
        return &m_gfx_resources;
    }
    std::optional<ShaderParamInfo*> ShaderParamBlock::find_param(ShaderParamId id) const {
        return m_shader ? m_shader->find_param(id) : std::nullopt;
    }

    ShaderParamBlockDesc ShaderParamBlock::desc() const {
        ShaderParamBlockDesc d;
        d.shader    = m_shader;
        d.space_idx = m_space;
        return d;
    }

}// namespace wmoge
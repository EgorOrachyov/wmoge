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

#include "grc_shader_param_block.hpp"

#include "core/log.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/grc_shader_param.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

#define WG_GRC_SET_VAR_BUFF                                        \
    const Status s = GrcShaderParamAccess(*this).set(param_id, v); \
    if (s) { dirty_buffers(); }                                    \
    return s

#define WG_GRC_SET_VAR_BIND                                        \
    const Status s = GrcShaderParamAccess(*this).set(param_id, v); \
    if (s) { dirty_set(); }                                        \
    return s

#define WG_GRC_GET_VAR_BUFF                                        \
    const Status s = GrcShaderParamAccess(*this).get(param_id, v); \
    return s

#define WG_GRC_GET_VAR_BIND                                        \
    const Status s = GrcShaderParamAccess(*this).get(param_id, v); \
    return s

    GrcShaderParamBlock::GrcShaderParamBlock(GrcShaderClass& shader_class, std::int16_t space_idx) {
        configure(shader_class, space_idx);
    }

    Status GrcShaderParamBlock::configure(GrcShaderClass& shader_class, std::int16_t space_idx) {
        m_class = &shader_class;
        m_space = space_idx;

        assert(space_idx < m_class->get_reflection().spaces.size());

        return reset_defaults();
    }

    Status GrcShaderParamBlock::reset_defaults() {
        if (!m_class) {
            WG_LOG_ERROR("param block not configured");
            return StatusCode::InvalidState;
        }

        const GrcShaderSpace& space = m_class->get_reflection().spaces[m_space];

        if (m_gfx_resources.empty()) {
            m_gfx_resources.resize(space.bindings.size());
        }

        for (std::int16_t i = 0; i < std::int16_t(space.bindings.size()); i++) {
            const GrcShaderBinding& binding = space.bindings[i];
            GfxDescBindPoint&       p       = m_gfx_resources[i].first;
            GfxDescBindValue&       v       = m_gfx_resources[i].second;

            p.binding       = i;
            p.array_element = 0;
            v.offset        = 0;

            switch (binding.binding) {
                case GrcShaderBindingType::InlineUniformBuffer:
                case GrcShaderBindingType::UniformBuffer:
                    p.type  = GfxBindingType::UniformBuffer;
                    v.range = binding.type->byte_size;
                    break;
                case GrcShaderBindingType::StorageBuffer:
                    p.type  = GfxBindingType::StorageBuffer;
                    v.range = binding.type->byte_size;
                    break;
                case GrcShaderBindingType::Sampler2d:
                case GrcShaderBindingType::Sampler2dArray:
                case GrcShaderBindingType::SamplerCube:
                    p.type     = GfxBindingType::SampledTexture;
                    v.resource = binding.default_tex.as<GfxResource>();
                    v.sampler  = binding.default_sampler;
                    break;
                case GrcShaderBindingType::StorageImage2d:
                    p.type = GfxBindingType::StorageImage;
                    break;
                default:
                    break;
            }
        }

        if (m_buffers.empty()) {
            for (const auto& buffer : m_class->get_reflection().buffers) {
                if (buffer.space == m_space) {
                    m_buffers.emplace_back(make_ref<Data>(buffer.size));
                }
            }
        }

        for (const auto& buffer : m_class->get_reflection().buffers) {
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

        return StatusCode::Ok;
    }

    Status GrcShaderParamBlock::validate(GfxDriver* driver, GfxCtx* ctx, Strid name) {
        if (!m_class) {
            WG_LOG_ERROR("param block not configured");
            return StatusCode::InvalidState;
        }
        if (!m_dirty_buffers && !m_dirty_set) {
            return StatusCode::Ok;
        }

        const GrcShaderSpace& space = m_class->get_reflection().spaces[m_space];

        if (m_dirty_buffers) {
            for (const auto& buffer : m_class->get_reflection().buffers) {
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

                    ctx->update_uniform_buffer(buffer, v.offset, v.range, src);
                }
            }
        }

        if (m_dirty_set) {
            for (std::int16_t i = 0; i < std::int16_t(m_gfx_resources.size()); i++) {
                const GfxDescBindPoint& p = m_gfx_resources[i].first;
                const GfxDescBindValue& v = m_gfx_resources[i].second;

                if (!v.resource) {
                    WG_LOG_ERROR("missing res setup of " << name << " binding=" << i);
                    return StatusCode::InvalidState;
                }
                if (p.type == GfxBindingType::SampledTexture && !v.sampler) {
                    WG_LOG_ERROR("missing sampler setup of " << name << " binding=" << i);
                    return StatusCode::InvalidState;
                }
            }

            m_gfx_set = driver->make_desc_set(m_gfx_resources, name);
        }

        m_dirty_buffers = 0;
        m_dirty_set     = 0;

        return StatusCode::Ok;
    }

    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, int v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, float v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Vec2f& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Vec3f& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Vec4f& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Vec2i& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Vec3i& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Vec4i& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Mat4x4f& v) { WG_GRC_SET_VAR_BUFF; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Ref<GfxTexture>& v) { WG_GRC_SET_VAR_BIND; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Ref<GfxSampler>& v) { WG_GRC_SET_VAR_BIND; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Ref<GfxUniformBuffer>& v) { WG_GRC_SET_VAR_BIND; }
    Status GrcShaderParamBlock::set_var(GrcShaderParamId param_id, const Ref<GfxStorageBuffer>& v) { WG_GRC_SET_VAR_BIND; }

    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, int& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, float& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Vec2f& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Vec3f& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Vec4f& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Vec2i& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Vec3i& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Vec4i& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Mat4x4f& v) { WG_GRC_GET_VAR_BUFF; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Ref<GfxTexture>& v) { WG_GRC_GET_VAR_BIND; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Ref<GfxSampler>& v) { WG_GRC_GET_VAR_BIND; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Ref<GfxUniformBuffer>& v) { WG_GRC_GET_VAR_BIND; }
    Status GrcShaderParamBlock::get_var(GrcShaderParamId param_id, Ref<GfxStorageBuffer>& v) { WG_GRC_GET_VAR_BIND; }

    Ref<Data>* GrcShaderParamBlock::get_buffer(std::int16_t buffer_idx) {
        return &m_buffers[buffer_idx];
    }
    Ref<Data>* GrcShaderParamBlock::get_buffer(std::int16_t space_idx, std::int16_t buffer_idx) {
        if (space_idx != m_space) {
            return nullptr;
        }
        return &m_buffers[buffer_idx];
    }
    GfxDescSetResources* GrcShaderParamBlock::get_gfx_resources() {
        return &m_gfx_resources;
    }
    GfxDescSetResources* GrcShaderParamBlock::get_gfx_resources(std::int16_t space_idx) {
        if (space_idx != m_space) {
            return nullptr;
        }
        return &m_gfx_resources;
    }

}// namespace wmoge
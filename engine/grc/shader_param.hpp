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
#include "core/data.hpp"
#include "core/log.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/shader_reflection.hpp"
#include "grc/shader_script.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"

#include <array>
#include <cinttypes>
#include <cstring>
#include <type_traits>

namespace wmoge {

    /**
     * @class GrcShaderParam
     * @brief Wrapper for a param modification inside a shader or a param block
    */
    class GrcShaderParam {
    public:
        GrcShaderParam() = default;
        GrcShaderParam(class GrcShader& shader, GrcShaderParamId param_id);
        GrcShaderParam(class GrcShaderParamBlock& block, GrcShaderParamId param_id);

        Status set_var(int v);
        Status set_var(float v);
        Status set_var(const Vec2f& v);
        Status set_var(const Vec3f& v);
        Status set_var(const Vec4f& v);
        Status set_var(const Vec2i& v);
        Status set_var(const Vec3i& v);
        Status set_var(const Vec4i& v);
        Status set_var(const Mat4x4f& v);
        Status set_var(const Ref<GfxTexture>& v);
        Status set_var(const Ref<GfxSampler>& v);
        Status set_var(const Ref<GfxUniformBuffer>& v);
        Status set_var(const Ref<GfxStorageBuffer>& v);

        Status get_var(int& v);
        Status get_var(float& v);
        Status get_var(Vec2f& v);
        Status get_var(Vec3f& v);
        Status get_var(Vec4f& v);
        Status get_var(Vec2i& v);
        Status get_var(Vec3i& v);
        Status get_var(Vec4i& v);
        Status get_var(Mat4x4f& v);
        Status get_var(Ref<GfxTexture>& v);
        Status get_var(Ref<GfxSampler>& v);
        Status get_var(Ref<GfxUniformBuffer>& v);
        Status get_var(Ref<GfxStorageBuffer>& v);

        [[nodiscard]] bool is_valid() const { return m_param_id.is_valid(); }
        [[nodiscard]] bool is_invalid() const { return m_param_id.is_invalid(); }

    private:
        class GrcShaderParamBlock* m_block = nullptr;
        GrcShaderParamId           m_param_id;
    };

    /**
     * @class GrcShaderParamAccess
     * @brief Helper to access shader params with compile time info
     * 
     * @tparam InfoProvider type of storage of params
    */
    template<typename StorageProvider>
    struct GrcShaderParamAccess {

        GrcShaderParamAccess(StorageProvider& provider) : provider(provider) {}

        template<typename T>
        Status set(GrcShaderParamId param_id, const T& v) {
            if (param_id.is_invalid()) {
                WG_LOG_ERROR("passed invalid param id");
                return StatusCode::InvalidParameter;
            }

            GrcShaderScript*                   script = provider.get_script();
            std::optional<GrcShaderParamInfo*> p_info = script->get_param_info(param_id);

            if (!p_info) {
                WG_LOG_ERROR("no such param id");
                return StatusCode::InvalidParameter;
            }

            GrcShaderParamInfo* param  = p_info.value();
            Ref<Data>*          buffer = nullptr;

            if (param->buffer != -1) {
                buffer = provider.get_buffer(param->space, param->buffer);
                if (!buffer) {
                    WG_LOG_ERROR("no such buffer");
                    return StatusCode::InvalidState;
                }

                const std::int16_t offset  = param->offset;
                const std::int16_t size    = param->elem_count * param->type->byte_size;
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

                std::memcpy(ptr + offset, (const std::uint16_t*) &to_copy, size);
                return StatusCode::Ok;
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
                return StatusCode::Ok;
            }
            if constexpr (std::is_same_v<T, Ref<GfxStorageBuffer>>) {
                bind_value.resource = v.template as<GfxResource>();
                return StatusCode::Ok;
            }
            if constexpr (std::is_same_v<T, Ref<GfxTexture>>) {
                bind_value.resource = v.template as<GfxResource>();
                return StatusCode::Ok;
            }
            if constexpr (std::is_same_v<T, Ref<GfxSampler>>) {
                bind_value.sampler = v;
                return StatusCode::Ok;
            }

            WG_LOG_ERROR("unsupported type of param");
            return StatusCode::Error;
        }

        template<typename T>
        Status get(GrcShaderParamId param_id, T& v) {
            if (param_id.is_invalid()) {
                WG_LOG_ERROR("passed invalid param id");
                return StatusCode::InvalidParameter;
            }

            GrcShaderScript*                   script = provider.get_script();
            std::optional<GrcShaderParamInfo*> p_info = script->get_param_info(param_id);

            if (!p_info) {
                WG_LOG_ERROR("no such param id");
                return StatusCode::InvalidParameter;
            }

            GrcShaderParamInfo* param  = p_info.value();
            Ref<Data>*          buffer = nullptr;

            if (param->buffer != -1) {
                buffer = provider.get_buffer(param->space, param->buffer);
                if (!buffer) {
                    WG_LOG_ERROR("no such buffer");
                    return StatusCode::InvalidState;
                }

                const std::int16_t  offset = param->offset;
                const std::int16_t  size   = param->elem_count * param->type->byte_size;
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
                return StatusCode::Ok;
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
                return StatusCode::Ok;
            }
            if constexpr (std::is_same_v<T, Ref<GfxStorageBuffer>>) {
                v = bind_value.resource.template cast<GfxStorageBuffer>();
                return StatusCode::Ok;
            }
            if constexpr (std::is_same_v<T, Ref<GfxTexture>>) {
                v = bind_value.resource.template cast<GfxTexture>();
                return StatusCode::Ok;
            }
            if constexpr (std::is_same_v<T, Ref<GfxSampler>>) {
                v = bind_value.sampler;
                return StatusCode::Ok;
            }

            WG_LOG_ERROR("unsupported type of param");
            return StatusCode::Error;
        }

        StorageProvider& provider;
    };

}// namespace wmoge
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

#include "core/data.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/grc_shader_class.hpp"
#include "grc/grc_shader_reflection.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"

#include <cinttypes>

namespace wmoge {

    /**
     * @class GrcShaderParamBlock
     * @brief Holds params set which can be bound to a shader for drawing
     * 
     * GrcShaderParamBlock is a smart and comfort abstraction over raw gfx
     * descriptor set management, required for dispatching shader on a gpu.
     * Prefere its usage instead of raw gfx descriptor sets.
     * 
     * GrcShaderParamBlock allows to set textures, buffers, and raw params using param
     * id and set methods. Data automatically packed internally into proper places. 
     * Default values setup automatically as well, based on shader class defenition.
     * 
     * GrcShaderParamBlock automatically creates and update buffers and descriptor sets on a gpu. 
     * After validation it can be directly inserted into desired descriptor set slot for drawing. 
    */
    class GrcShaderParamBlock {
    public:
        /**
         * Construct empty block with no setup.
         * Configure it using `configure` method.
        */
        GrcShaderParamBlock() = default;

        /**
         * @brief Creates params block with default setup for given shader class and space.
         * 
         * @param shader_class Class of shader intended to be used with block
         * @param space_idx Index of space to be used with this block
        */
        GrcShaderParamBlock(GrcShaderClass& shader_class, std::int16_t space_idx);

        ~GrcShaderParamBlock() = default;

        Status configure(GrcShaderClass& shader_class, std::int16_t space_idx);
        Status reset_defaults();
        Status validate(class GfxDriver* driver, class GfxCtx* ctx, Strid name);

        Status set_var(GrcShaderParamId param_id, int v);
        Status set_var(GrcShaderParamId param_id, float v);
        Status set_var(GrcShaderParamId param_id, const Vec2f& v);
        Status set_var(GrcShaderParamId param_id, const Vec3f& v);
        Status set_var(GrcShaderParamId param_id, const Vec4f& v);
        Status set_var(GrcShaderParamId param_id, const Vec2i& v);
        Status set_var(GrcShaderParamId param_id, const Vec3i& v);
        Status set_var(GrcShaderParamId param_id, const Vec4i& v);
        Status set_var(GrcShaderParamId param_id, const Mat4x4f& v);
        Status set_var(GrcShaderParamId param_id, const Ref<GfxTexture>& v);
        Status set_var(GrcShaderParamId param_id, const Ref<GfxSampler>& v);
        Status set_var(GrcShaderParamId param_id, const Ref<GfxUniformBuffer>& v);
        Status set_var(GrcShaderParamId param_id, const Ref<GfxStorageBuffer>& v);

        Status get_var(GrcShaderParamId param_id, int& v);
        Status get_var(GrcShaderParamId param_id, float& v);
        Status get_var(GrcShaderParamId param_id, Vec2f& v);
        Status get_var(GrcShaderParamId param_id, Vec3f& v);
        Status get_var(GrcShaderParamId param_id, Vec4f& v);
        Status get_var(GrcShaderParamId param_id, Vec2i& v);
        Status get_var(GrcShaderParamId param_id, Vec3i& v);
        Status get_var(GrcShaderParamId param_id, Vec4i& v);
        Status get_var(GrcShaderParamId param_id, Mat4x4f& v);
        Status get_var(GrcShaderParamId param_id, Ref<GfxTexture>& v);
        Status get_var(GrcShaderParamId param_id, Ref<GfxSampler>& v);
        Status get_var(GrcShaderParamId param_id, Ref<GfxUniformBuffer>& v);
        Status get_var(GrcShaderParamId param_id, Ref<GfxStorageBuffer>& v);

        [[nodiscard]] const Ref<GfxDescSet>& get_gfx_set() { return m_gfx_set; }
        [[nodiscard]] GrcShaderClass*        get_class() { return m_class; }
        [[nodiscard]] std::int16_t           get_space() { return m_space; }
        [[nodiscard]] Ref<Data>*             get_buffer(std::int16_t buffer_idx);
        [[nodiscard]] Ref<Data>*             get_buffer(std::int16_t space_idx, std::int16_t buffer_idx);
        [[nodiscard]] GfxDescSetResources*   get_gfx_resources();
        [[nodiscard]] GfxDescSetResources*   get_gfx_resources(std::int16_t space_idx);

    private:
        void dirty_buffers() { m_dirty_buffers = 1; }
        void dirty_set() { m_dirty_set = 1; }

    private:
        GfxDescSetResources       m_gfx_resources;
        Ref<GfxDescSet>           m_gfx_set;
        fast_vector<Ref<Data>, 1> m_buffers;
        GrcShaderClass*           m_class         = nullptr;
        std::int16_t              m_space         = -1;
        std::int8_t               m_dirty_buffers = 1;
        std::int8_t               m_dirty_set     = 1;
    };

}// namespace wmoge
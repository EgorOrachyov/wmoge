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
#include "math/mat.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class ShaderParam
     * @brief Wrapper for a param modification inside a shader or a param block
    */
    class ShaderParam {
    public:
        ShaderParam() = default;
        ShaderParam(class ShaderInstance& shader, ShaderParamId param_id);
        ShaderParam(class ShaderParamBlock& block, ShaderParamId param_id);

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
        class ShaderParamBlock* m_block = nullptr;
        ShaderParamId           m_param_id;
    };

}// namespace wmoge
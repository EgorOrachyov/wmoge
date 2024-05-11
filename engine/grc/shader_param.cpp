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

#include "shader_param.hpp"

#include "grc/shader.hpp"
#include "grc/shader_instance.hpp"
#include "grc/shader_param_block.hpp"

namespace wmoge {

#define WG_GRC_SET_VAR_BUFF                            \
    if (!m_block) { return StatusCode::InvalidState; } \
    const Status s = m_block->set_var(m_param_id, v);  \
    return s

#define WG_GRC_SET_VAR_BIND                            \
    if (!m_block) { return StatusCode::InvalidState; } \
    const Status s = m_block->set_var(m_param_id, v);  \
    return s

#define WG_GRC_GET_VAR_BUFF                            \
    if (!m_block) { return StatusCode::InvalidState; } \
    const Status s = m_block->get_var(m_param_id, v);  \
    return s

#define WG_GRC_GET_VAR_BIND                            \
    if (!m_block) { return StatusCode::InvalidState; } \
    const Status s = m_block->get_var(m_param_id, v);  \
    return s

    ShaderParam::ShaderParam(ShaderInstance& shader, ShaderParamId param_id) {
    }

    ShaderParam::ShaderParam(ShaderParamBlock& block, ShaderParamId param_id) {
        m_block    = &block;
        m_param_id = param_id;
    }

    Status ShaderParam::set_var(int v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(float v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Vec2f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Vec3f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Vec4f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Vec2i& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Vec3i& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Vec4i& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Mat4x4f& v) { WG_GRC_SET_VAR_BUFF; }
    Status ShaderParam::set_var(const Ref<GfxTexture>& v) { WG_GRC_SET_VAR_BIND; }
    Status ShaderParam::set_var(const Ref<GfxSampler>& v) { WG_GRC_SET_VAR_BIND; }
    Status ShaderParam::set_var(const Ref<GfxUniformBuffer>& v) { WG_GRC_SET_VAR_BIND; }
    Status ShaderParam::set_var(const Ref<GfxStorageBuffer>& v) { WG_GRC_SET_VAR_BIND; }

    Status ShaderParam::get_var(int& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(float& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Vec2f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Vec3f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Vec4f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Vec2i& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Vec3i& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Vec4i& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Mat4x4f& v) { WG_GRC_GET_VAR_BUFF; }
    Status ShaderParam::get_var(Ref<GfxTexture>& v) { WG_GRC_GET_VAR_BIND; }
    Status ShaderParam::get_var(Ref<GfxSampler>& v) { WG_GRC_GET_VAR_BIND; }
    Status ShaderParam::get_var(Ref<GfxUniformBuffer>& v) { WG_GRC_GET_VAR_BIND; }
    Status ShaderParam::get_var(Ref<GfxStorageBuffer>& v) { WG_GRC_GET_VAR_BIND; }

}// namespace wmoge
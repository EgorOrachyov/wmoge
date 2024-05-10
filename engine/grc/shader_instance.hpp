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

#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_interface.hpp"
#include "grc/shader_param_block.hpp"

namespace wmoge {

    /**
     * @class ShaderInstance
     * @brief An instance of the shader for drawing
    */
    class ShaderInstance {
    public:
        ShaderInstance()  = default;
        ~ShaderInstance() = default;

        ShaderInstance(ShaderInterface& shader, Strid name);

        Status init(ShaderInterface& shader, Strid name);
        Status restore_defaults();

        Status begin_pass(const Strid& pass, class GfxDriver* driver, class GfxCtx* ctx);
        void   draw(int vertex_count, int base_vertex, int instance_count);
        void   draw_indexed(int index_count, int base_vertex, int instance_count);
        void   dispatch(Vec3i group_count);
        void   end_pass();

        Status set_technique(const Strid& name);
        Status set_technique(std::int16_t idx);
        Status set_option(Strid option, Strid variant);
        Status set_option(std::int16_t pass_idx, Strid option, Strid variant);

        Status set_var(ShaderParamId param_id, int v);
        Status set_var(ShaderParamId param_id, float v);
        Status set_var(ShaderParamId param_id, const Vec2f& v);
        Status set_var(ShaderParamId param_id, const Vec3f& v);
        Status set_var(ShaderParamId param_id, const Vec4f& v);
        Status set_var(ShaderParamId param_id, const Vec2i& v);
        Status set_var(ShaderParamId param_id, const Vec3i& v);
        Status set_var(ShaderParamId param_id, const Vec4i& v);
        Status set_var(ShaderParamId param_id, const Mat4x4f& v);
        Status set_var(ShaderParamId param_id, const Ref<GfxTexture>& v);
        Status set_var(ShaderParamId param_id, const Ref<GfxSampler>& v);
        Status set_var(ShaderParamId param_id, const Ref<GfxUniformBuffer>& v);
        Status set_var(ShaderParamId param_id, const Ref<GfxStorageBuffer>& v);

        Status get_var(ShaderParamId param_id, int& v);
        Status get_var(ShaderParamId param_id, float& v);
        Status get_var(ShaderParamId param_id, Vec2f& v);
        Status get_var(ShaderParamId param_id, Vec3f& v);
        Status get_var(ShaderParamId param_id, Vec4f& v);
        Status get_var(ShaderParamId param_id, Vec2i& v);
        Status get_var(ShaderParamId param_id, Vec3i& v);
        Status get_var(ShaderParamId param_id, Vec4i& v);
        Status get_var(ShaderParamId param_id, Mat4x4f& v);
        Status get_var(ShaderParamId param_id, Ref<GfxTexture>& v);
        Status get_var(ShaderParamId param_id, Ref<GfxSampler>& v);
        Status get_var(ShaderParamId param_id, Ref<GfxUniformBuffer>& v);
        Status get_var(ShaderParamId param_id, Ref<GfxStorageBuffer>& v);

    private:
        buffered_vector<ShaderParamBlock> m_param_blocks;
        ShaderInterface*                  m_shader = nullptr;
        Strid                             m_name;
        std::int16_t                      m_technique_idx = -1;
    };

}// namespace wmoge
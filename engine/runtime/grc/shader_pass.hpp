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
#include "grc/shader_param_block.hpp"
#include "grc/shader_reflection.hpp"

namespace wmoge {

    /**
     * @class ShaderPass
     * @brief An instance of a shader pass to select options, configure params and dispatch gpu commands
    */
    class ShaderPass {
    public:
        ShaderPass(class Shader& shader);

        void set_param_block(std::int16_t idx, Ref<ShaderParamBlock> block);
        void set_technique(std::int16_t idx);
        void set_technique(const Strid& name);
        void set_pass(std::int16_t idx);
        void set_pass(const Strid& name);
        void set_attribs(int buffer, GfxVertAttribs attribs, std::optional<GfxVertAttribs> layout = std::nullopt);
        void set_attribs_instanced(int buffer, GfxVertAttribs attribs, std::optional<GfxVertAttribs> layout = std::nullopt);
        void set_prim_type(GfxPrimType prim_type);
        void set_rs(const RasterState& rs);
        void set_ds(const DepthStencilState& ds);
        void set_bs(const BlendState& bs);

        Status configure(class GfxCtx* context);

    private:
        Status configure_graphics(class GfxCtx* context);
        Status configure_compute(class GfxCtx* context);

    private:
        buffered_vector<Ref<ShaderParamBlock>>   m_params;
        PipelineState                            m_pipeline_state;
        ShaderPermutation                        m_permutation;
        GfxVertAttribsStreams                    m_vert_attribs;
        GfxVertAttribsStreams                    m_vert_layout;
        std::bitset<GfxLimits::MAX_VERT_STREAMS> m_vert_instanced;
        GfxPrimType                              m_prim_type = GfxPrimType::Triangles;
        class Shader*                            m_shader;
        const ShaderTechniqueInfo*               m_technique = nullptr;
        const ShaderPassInfo*                    m_pass      = nullptr;
    };

}// namespace wmoge
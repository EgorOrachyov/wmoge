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
#include "grc/grc_shader_reflection.hpp"
#include "grc/grc_shader_script.hpp"

namespace wmoge {

    /**
     * @class GrcShaderPermutation
     * @brief Defines a particular variant of a compiled shader
    */
    struct GrcShaderPermutation {
        static constexpr int MAX_OPTIONS = 64;

        std::bitset<MAX_OPTIONS> options;
        std::int16_t             technique_idx;
        std::int16_t             pass_idx;
        GfxVertAttribs           vert_attribs;
    };

    /**
     * @class GrcShader
     * @brief Shader is an instance of shader class for rendering
     * 
     * GrcShader in an instance of shader class. It allows to select a particular pass 
     * and options for program, configure params, and render geometry, dispatch compute
     * shader or configure a particular shader pass.
     * 
     * GrcShader provides pass and options info for gpu program compilation. In some
     * cases it may require vertex format as well for rendering passes. 
     * 
     * GrcShader may be used itself for internal rendering, or a base for compilation
     * of passes for optimized engine models/meshes rendering.
    */
    class GrcShader {
    public:
    private:
        std::int16_t     m_techique_idx = -1;
        GrcShaderScript* m_class;
    };

}// namespace wmoge
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
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"

#include <vector>

namespace wmoge {

    /** 
     * @class GlslInputFile
     * @brief Input file to compile 
     */
    struct GlslInputFile {
        std::string     source_code;
        std::string     entry_point;
        GfxShaderModule module_type;
    };

    /** 
     * @class GlslCompilerInput
     * @brief Compiler input 
     */
    struct GlslCompilerInput {
        buffered_vector<GlslInputFile> files;
        Strid                          name;
        bool                           disable_otimizer = false;
        bool                           optimize_size    = false;
        bool                           validate         = true;
    };

    /** 
     * @class GlslCompilerOutput
     * @brief Compiler result
    */
    struct GlslCompilerOutput {
        buffered_vector<Ref<Data>>      bytecode;
        buffered_vector<std::string, 1> errors;
    };

    /**
     * @class GlslShaderCompiler
     * @brief Glslang based compiler for shaders
    */
    class GlslShaderCompiler {
    public:
        GlslShaderCompiler();
        ~GlslShaderCompiler();

        Status compile(const GlslCompilerInput& input, GlslCompilerOutput& output);
    };

}// namespace wmoge
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
#include "core/flat_map.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_resource.hpp"

#include <array>
#include <cinttypes>
#include <optional>

namespace wmoge {

    /**
     * @class GfxShader
     * @brief Compiled single gpu program module
     */
    class GfxShader : public GfxResource {
    public:
        ~GfxShader() override = default;

        [[nodiscard]] const Ref<Data>& get_bytecode() const { return m_bytecode; }
        [[nodiscard]] GfxShaderModule  get_module_type() const { return m_module_type; }

    protected:
        Ref<Data>       m_bytecode;
        GfxShaderModule m_module_type;
    };

    /**
     * @brief Desc to create program
    */
    using GfxShaderProgramDesc = buffered_vector<Ref<GfxShader>, 2>;

    /**
     * @class GfxShaderProgram
     * @brief Compiled and linked full shader program with all stages
     * 
     * Shaders consists of a number of stages for execution.
     * Shader can be created from any engine thread. 
     *
     * Possible shader stages sets:
     *  - vertex and fragment for classic rendering
     *  - compute for computational pipeline
    */
    class GfxShaderProgram : public GfxResource {
    public:
        ~GfxShaderProgram() override = default;

        [[nodiscard]] GfxShaderProgramDesc get_desc() const { return m_desc; }

    protected:
        GfxShaderProgramDesc m_desc;
    };

}// namespace wmoge
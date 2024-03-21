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

#include "core/status.hpp"
#include "grc/grc_shader_reflection.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    /**
     * @class GrcShaderClass
     * @brief Reprsents a particular shader program class
     * 
     * GrcShaderClass is a high level representation of a shading program.
     * It provides a connection between raw glsl sources code of a shader,
     * material and engine gfx module for runtime usage.
     * 
     * GrcShaderClass provides info about a particular shader type. It provides
     * layout information, parameters and structures layout, defines and
     * compilations options, constants and includes, and provides hot-reloading
     * mechanism for debugging. 
     * 
     * GrcShaderClass is a `template` shader for drawing with pre-defined interface.
     * It is not suitable for rendering. In order to get a concrete instance of 
     * compiled gpu program, pass and options must be provided from GrcShader.
    */
    class GrcShaderClass {
    public:
        GrcShaderClass(GrcShaderReflection&& reflection);

        int    set_idx(int idx);
        Status reload_sources(const std::string& folder, FileSystem* fs);
        Status fill_layout(GfxDescSetLayoutDesc& desc, int space) const;
        bool   has_dependency(const Strid& dependency) const;
        bool   has_space(GrcShaderSpaceType space_type) const;

        [[nodiscard]] const GrcShaderReflection& get_reflection() const { return m_reflection; }

    private:
        GrcShaderReflection m_reflection;
        int                 m_idx = -1;
    };

}// namespace wmoge
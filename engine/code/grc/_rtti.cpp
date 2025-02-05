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

#include "_rtti.hpp"

#include "grc/font.hpp"
#include "grc/image.hpp"
#include "grc/pso_file.hpp"
#include "grc/shader.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_reflection.hpp"
#include "grc/texture.hpp"

namespace wmoge {

    void rtti_grc() {
        rtti_type<ShaderReflection>();
        rtti_type<ShaderFileOption>();
        rtti_type<ShaderFileParam>();
        rtti_type<ShaderFileParamBlock>();
        rtti_type<ShaderFilePass>();
        rtti_type<ShaderFileTechnique>();
        rtti_type<ShaderFileSource>();
        rtti_type<ShaderFile>();
        rtti_type<Shader>();
        rtti_type<Shader>();
        rtti_type<ShaderPermutation>();
        rtti_type<ShaderCompiler>();
        rtti_type<FilePsoVertFormat>();
        rtti_type<FilePsoRenderPass>();
        rtti_type<FilePsoLayout>();
        rtti_type<FilePsoShader>();
        rtti_type<FilePsoStateGraphics>();
        rtti_type<FilePsoStateCompute>();
        rtti_type<FilePsoData>();
        rtti_type<Image>();
        rtti_type<Texture>();
        rtti_type<Texture2d>();
        rtti_type<TextureCube>();
        rtti_type<Font>();
    }

}// namespace wmoge
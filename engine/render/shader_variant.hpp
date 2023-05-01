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

#ifndef WMOGE_SHADER_VARIATION_HPP
#define WMOGE_SHADER_VARIATION_HPP

#include "core/string_id.hpp"
#include "gfx/gfx_shader.hpp"

#include <string>

namespace wmoge {

    /**
     * @class ShaderVariant
     * @brief A particular compiled variation of a shader resource
     *
     * Shader variant stores an actual gfx shader item, which
     * can be bound to the graphics pipeline for the rendering.
     */
    class ShaderVariant final {
    public:
        ref_ptr<class Shader>     get_owner();
        const StringId&           get_key();
        const std::size_t&        get_hash();
        const ref_ptr<GfxShader>& get_gfx_shader();
        int                       get_material_first_texture();
        int                       get_material_first_buffer();

    private:
        friend class Shader;
        friend class ShaderBuilder;

        ref_ptr<GfxShader> m_gfx_shader;
        class Shader*      m_owner = nullptr;
        StringId           m_key;
        std::size_t        m_hash;
        int                m_material_first_texture = -1;
        int                m_material_first_buffer  = -1;
    };

}// namespace wmoge

#endif//WMOGE_SHADER_VARIATION_HPP

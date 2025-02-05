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

#include "render/interop.hpp"

namespace wmoge {

    /**
     * @class ShaderTable
     * @brief Table with common render system shaders
     * 
     * Shader table provides access to built-in engine shaders, 
     * declared in engine shaders folder. This shader table shaders
     * are a type-safe compile-time wrapper for engine shaders. 
     * It can be used to reduce boilerplate in case of writing
     * engine related rendering code.
     */
    class ShaderTable {
    public:
        ShaderTable() = default;

        Status reflect_types(class ShaderManager* asset_manager);
        Status load_shaders(class AssetManager* asset_manager);

        [[nodiscard]] const ShaderAuxDraw* aux_draw() const { return &m_aux_draw; }
        [[nodiscard]] const ShaderBlit*    blit() const { return &m_blit; }
        [[nodiscard]] const ShaderCanvas*  canvas() const { return &m_canvas; }
        [[nodiscard]] const ShaderFill*    fill() const { return &m_fill; }

    private:
        ShaderAuxDraw m_aux_draw;
        ShaderBlit    m_blit;
        ShaderCanvas  m_canvas;
        ShaderFill    m_fill;
    };

}// namespace wmoge
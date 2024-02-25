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

#include "gfx/gfx_defs.hpp"

namespace wmoge {

    /**
     * @class GfxPassDesc
     * @brief Base class for a description to setup a gfx pass
     * 
     * Gfx pass descriptor is a minimal self-contained representation,
     * required to setup a GfxPass for rendering.
     * 
     * Description can be created from any system and used to automate
     * setup of required for the rendering data:
     *  - gather required defines for the shader
     *  - compile gfx shader items
     *  - setup required vertex format and render state
     *  - compile pipeline state object
    */
    class GfxPassDesc {
    public:
    };

    /**
     * @class GfxPass
     * @brief Base class for any GPU shaders
     * 
     * Gfx pass is a high level shading program representation. It provides connection
     * between a shader, written using glsl in engine source code, optional user defined
     * material and a low-level engine gfx api.
     *
     * Gfx pass provides info about required pipeline layout, allows to obtain final
     * shader source code, provides defines info and etc.
    */
    class GfxPass {
    public:
    };

}// namespace wmoge
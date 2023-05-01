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

#ifndef WMOGE_DRAW_CMD_COMPILER_HPP
#define WMOGE_DRAW_CMD_COMPILER_HPP

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "render/draw_cmd.hpp"
#include "render/draw_primitive.hpp"

namespace wmoge {

    /**
     * @class DrawCmdCompiler
     * @brief Base class responsible for primitives to draw cmd compiling
     *
     * Compiler is responsible for converting high-level abstract draw primitives
     * to lower-level optimized native Gfx specific draw cmds.
     *
     * Compiler may have additional state for config. Compilation may be done
     * asynchronous using tasks and task manager for speed up.
     */
    class DrawCmdCompiler {
    public:
        DrawCmdCompiler();

        bool compile(const DrawPrimitive& primitive, ArrayView<DrawCmd*> cmds);

        void set_engine(class Engine* engine);
        void set_gfx_driver(class GfxDriver* gfx_driver);
        void set_render_engine(class RenderEngine* render_engine);
        void set_render_scene(class RenderScene* render_scene);

    private:
        fast_vector<std::string> m_defines;
        class Engine*            m_engine        = nullptr;
        class GfxDriver*         m_gfx_driver    = nullptr;
        class RenderEngine*      m_render_engine = nullptr;
        class RenderScene*       m_render_scene  = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_DRAW_CMD_COMPILER_HPP

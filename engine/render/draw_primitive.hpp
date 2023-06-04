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

#ifndef WMOGE_DRAW_PRIMITIVE_HPP
#define WMOGE_DRAW_PRIMITIVE_HPP

#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "render/draw_cmd.hpp"
#include "render/draw_queue.hpp"
#include "resource/material.hpp"
#include "resource/shader.hpp"

#include <mutex>
#include <type_traits>
#include <vector>

namespace wmoge {

    /**
     * @class DrawPrimitive
     * @brief Base class for a primitive to be drawn with a material and vertex data
     *
     * Draw primitive is high-level primitive for a communication with a rendering
     * engine, scene and views. It captures single chuck of data to be draw with
     * fixed vertex, index and material setup, as well as separate draw constants.
     *
     * Primitive itself cannot be rendered. In order to be rendered, it must be
     * compiled in an efficient Gfx driver friendly `DrawCmd` with complete setup
     * for rendering. Single primitive can be compiled to a set of commands,
     * depending on number of passes to draw.
     *
     * Primitive compilation to a set of commands is a costly operations. Try
     * to avoid it. Prefer persistent primitives creation with cached draw
     * commands to be drawn.
     *
     * @note Primitive can be understood as a single Vertex Array Object (VAO) in OpenGL
     *       terminology with respect to having additional bindings from material.
     *
     * @note Primitive is a pod structure, it does not owns and controls the life
     *       time of stored buffers, materials, etc., it must be controlled outside.
     *
     * @see DrawCmd
     * @see DrawCmdQueue
     */
    struct alignas(64) DrawPrimitive {
        /** @brief actual draw call setup */
        DrawParams draw_params{};
        /** @brief up to 4 inline streams with vertex data */
        DrawVertexBuffers vertices;
        /** @brief optional indices stream */
        DrawIndexBuffer indices;
        /** @brief constants to configure draw */
        DrawUniformBuffer constants;
        /** @brief layout of input vertex data */
        GfxVertFormat* vert_format;
        /** @brief material proxy to obtain source material, shader, pipeline, bindings, etc. */
        RenderMaterial* material = nullptr;
        /** @brief passes to be drawn in */
        DrawPassMask draw_pass;
        /** @brief attributes of a mesh to use in shader */
        GfxVertAttribs attribs;
        /** @brief type of primitives to draw */
        GfxPrimType prim_type = GfxPrimType::Triangles;
        /** @brief name for debug purposes */
        StringId name;
    };

    static_assert(sizeof(DrawPrimitive) <= 192, "draw primitive to large, affects performance");
    static_assert(std::is_trivially_destructible_v<DrawPrimitive>, "primitive must be trivially destructible for fast allocation");

    /**
     * @class DrawPrimitiveCollector
     * @brief Collect primitives to be compiled and drawn later
     */
    class DrawPrimitiveCollector {
    public:
        void push(DrawPrimitive* primitive);
        void reserve(std::size_t size);
        void clear();

        std::vector<DrawPrimitive*>&       get_primitives();
        const std::vector<DrawPrimitive*>& get_primitives() const;

    private:
        std::vector<DrawPrimitive*> m_primitives;
        std::mutex                  m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_DRAW_PRIMITIVE_HPP

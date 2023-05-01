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

#ifndef WMOGE_DRAW_CMD_HPP
#define WMOGE_DRAW_CMD_HPP

#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_texture.hpp"
#include "render/render_material.hpp"

#include <array>
#include <type_traits>

namespace wmoge {

    /**
     * @class DrawPass
     * @brief Id of pass for rendering
     */
    enum class DrawPass {
        DepthPass            = 0,
        ColorPass            = 1,
        ColorTransparentPass = 2,
        Overlay2dPass        = 3,
        Max                  = 4
    };

    /**
     * @class DrawPassMask
     * @brief Mask of passes to draw
     */
    using DrawPassMask = Mask<DrawPass>;

    /**
     * @brief DrawPassConsts
     * @class Constants describing how to process commands in draw
     */
    struct DrawPassConsts {
        static constexpr const int DRAW_SET_PER_PASS     = 0;
        static constexpr const int DRAW_SET_PER_MATERIAL = 1;
        static constexpr const int DRAW_SET_PER_DRAW     = 2;
    };

    /**
     * @class DrawParams
     * @brief Params to issue a single draw
     */
    struct DrawParams {
        int vertex_count   = -1;
        int index_count    = -1;
        int base_vertex    = -1;
        int instance_count = -1;
    };

    /**
     * @class DrawVertexBuffers
     * @brief Vertex buffers setup
     */
    struct DrawVertexBuffers {
        /** @brief Limit inline up to 4 buffers, 3 for per vert and 1 per inst */
        static const int                               MAX_VERTEX_BUFFERS = 4;
        std::array<GfxVertBuffer*, MAX_VERTEX_BUFFERS> buffers{};
        std::array<int, MAX_VERTEX_BUFFERS>            offsets{};
    };

    /**
     * @class DrawIndexBuffer
     * @brief Index buffer setup
     */
    struct DrawIndexBuffer {
        GfxIndexBuffer* buffer     = nullptr;
        int             offset     = -1;
        GfxIndexType    index_type = GfxIndexType::Uint32;
    };

    /**
     * @class DrawUniformBuffer
     * @brief Uniform buffer setup for rendering
     */
    struct DrawUniformBuffer {
        GfxUniformBuffer* buffer   = nullptr;
        int               offset   = -1;
        int               range    = -1;
        int               location = -1;
    };

    /**
     * @class DrawMaterialBindings
     * @brief Info from shader to bind material resource to required slots
     */
    struct DrawMaterialBindings {
        int first_texture = -1;
        int first_buffer  = -1;
    };

    /**
     * @class DrawCmd
     * @brief Fully describing a single draw call including all required params
     *
     * Draw command is a lower-level interface to communicate with a rendering
     * pipeline of the engine. Command is state-less, it contains all information
     * required for the rendering and executing it inside a queue `DrawCmdQueue`
     * in a respective rendering pass `DrawPass`.
     *
     * Compilation of a command may be a costly operation. Thus, prefer allocating
     * persistent commands and cache them inside yours rendered objects.
     *
     * @note Command is a pod structure, it does not owns and controls the life
     *       time of stored buffers, pipelines, etc., it must be controlled outside.
     */
    struct alignas(64) DrawCmd {
        /** @brief actual draw call setup */
        DrawParams draw_params{};
        /** @brief up to 4 inline streams with vertex data */
        DrawVertexBuffers vertices;
        /** @brief optional indices stream */
        DrawIndexBuffer indices;
        /** @brief constants to configure draw */
        DrawUniformBuffer constants;
        /** @brief config how to bind material params */
        DrawMaterialBindings bindings;
        /** @brief bindings of material */
        RenderMaterial* material = nullptr;
        /** @brief fully compiled pipeline */
        GfxPipeline* pipeline = nullptr;
    };

    static_assert(sizeof(DrawCmd) <= 128, "draw command to large, affects performance");
    static_assert(std::is_trivially_destructible_v<DrawCmd>, "command must be trivially destructible for fast allocation");

}// namespace wmoge

#endif//WMOGE_DRAW_CMD_HPP

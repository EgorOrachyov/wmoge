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

#include "core/array_view.hpp"
#include "core/buffered_vector.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/task_manager.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/color.hpp"
#include "mesh/mesh_batch.hpp"
#include "mesh/mesh_pass.hpp"
#include "platform/window.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/camera.hpp"
#include "render/canvas.hpp"
#include "render/gpu_buffer.hpp"
#include "render/render_defs.hpp"
#include "render/render_queue.hpp"
#include "render/render_scene.hpp"

#include <array>
#include <optional>

namespace wmoge {

    /**
     * @class RenderView
     * @brief Holds data required to render a single view
    */
    struct RenderView {
        static constexpr int QUEUE_COUNT = MESH_PASSES_TOTAL;

        std::array<RenderQueue, QUEUE_COUNT> queues;
        Ref<GfxUniformBuffer>                view_data;
        Ref<GfxDescSet>                      view_set;
        int                                  index = 0;
    };

    /**
     * @class RenderEngine
     * @brief Rendering engine
    */
    class RenderEngine {
    public:
        RenderEngine(class IocContainer* ioc);

    private:
    };

}// namespace wmoge
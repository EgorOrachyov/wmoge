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

#ifndef WMOGE_RENDER_QUEUE_HPP
#define WMOGE_RENDER_QUEUE_HPP

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"

#include <cinttypes>
#include <mutex>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class RenderCmdKey
     * @brief Key to sort commands for efficient execution
     */
    struct RenderCmdKey {
        std::uint64_t value;
    };

    static_assert(std::is_trivially_destructible_v<RenderCmdKey>, "render cmd key must be trivial as possible");
    static_assert(sizeof(RenderCmdKey) == sizeof(std::uint64_t), "render cmd key must fit 8 bytes");

    /**
     * @class RenderCmd
     * @brief POD-Command representing single draw call
     */
    struct RenderCmd {
        GfxVertBuffersSetup vert_buffers;
        GfxIndexBufferSetup index_setup;
        GfxDescSet*         desc_sets[GfxLimits::MAX_DESC_SETS]{};
        GfxPipeline*        pipeline = nullptr;
        GfxDrawCall         call_params;
    };

    static_assert(std::is_trivially_destructible_v<RenderCmd>, "render cmd must be trivial as possible");
    static_assert(sizeof(RenderCmd) <= 128, "render cmd must fit 128 bytes");

    /**
     * @class RenderQueue
     * @brief Thread-safe queue to collect and sort draw commands for gfx submission
     */
    struct RenderQueue {
    public:
        void push(RenderCmdKey key, const RenderCmd& cmd);
        void clear();
        void sort();
        int  execute(GfxCtx* gfx_ctx);

        [[nodiscard]] std::size_t      size() const;
        [[nodiscard]] const RenderCmd& cmd(std::size_t index) const;

    private:
        std::vector<std::pair<RenderCmdKey, int>> m_queue;
        std::vector<RenderCmd>                    m_buffer;
        std::mutex                                m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_QUEUE_HPP

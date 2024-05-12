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

#include "render_queue.hpp"

#include "profiler/profiler.hpp"

#include <algorithm>
#include <cassert>

namespace wmoge {

    RenderCmd* RenderCmdAllocator::allocate() {
        std::lock_guard guard(m_mutex);
        return &m_buffer.emplace_back(RenderCmd());
    }

    void RenderCmdAllocator::clear() {
        std::lock_guard guard(m_mutex);
        m_buffer.clear();
    }

    void RenderQueue::push(const SortableRenderCmd& cmd) {
        std::lock_guard guard(m_mutex);
        m_queue.emplace_back(cmd);
    }

    void RenderQueue::clear() {
        std::lock_guard guard(m_mutex);
        m_queue.clear();
    }

    void RenderQueue::sort() {
        WG_AUTO_PROFILE_RENDER("RenderQueue::sort");
        std::sort(m_queue.begin(), m_queue.end());
    }

    int RenderQueue::execute(GfxCtx* gfx_ctx) const {
        WG_AUTO_PROFILE_RENDER("RenderQueue::execute");

        int num_executed = 0;
        int num_total    = int(m_queue.size());

        GfxPsoGraphics* bound_pipeline = nullptr;

        for (int i = 0; i < num_total; i++) {
            const SortableRenderCmd& sortable_cmd = m_queue[i];
            const RenderCmd&         cmd          = *sortable_cmd.cmd;

            const GfxVertBuffersSetup& vert_buffers = cmd.vert_buffers;
            const GfxIndexBufferSetup& index_setup  = cmd.index_setup;
            const GfxDrawCall&         call_params  = cmd.call_params;

            if (bound_pipeline != cmd.pipeline) {
                if (false /* !gfx_ctx->bind_pso(Ref<GfxPsoGraphics>(cmd.pipeline)) */) {
                    continue;
                }
                bound_pipeline = cmd.pipeline;
            }

            for (int i = 0; i < RenderCmd::NUM_DESC_SETS; i++) {
                if (cmd.desc_sets[i] && cmd.desc_sets_slots[i] >= 0) {
                    gfx_ctx->bind_desc_set(Ref<GfxDescSet>(cmd.desc_sets[i]), cmd.desc_sets_slots[i]);
                }
            }

            for (int i = 0; i < GfxLimits::MAX_VERT_BUFFERS; i++) {
                if (!vert_buffers.buffers[i]) { break; }
                gfx_ctx->bind_vert_buffer(Ref<GfxVertBuffer>(vert_buffers.buffers[i]), i, vert_buffers.offsets[i]);
            }

            if (cmd.index_setup.buffer) {
                gfx_ctx->bind_index_buffer(Ref<GfxIndexBuffer>(index_setup.buffer), index_setup.index_type, index_setup.offset);
                gfx_ctx->draw_indexed(call_params.count, call_params.base, call_params.instances);
            } else {
                gfx_ctx->draw(call_params.count, call_params.base, call_params.instances);
            }

            num_executed += 1;
        }

        return num_executed;
    }

    std::vector<SortableRenderCmd>& RenderQueue::get_queue() {
        return m_queue;
    }

    const SortableRenderCmd& RenderQueue::get_cmd(std::size_t index) const {
        assert(index < m_queue.size());
        return m_queue[index];
    }

    std::size_t RenderQueue::get_size() const {
        return m_queue.size();
    }

    bool RenderQueue::is_empty() const {
        return m_queue.empty();
    }

}// namespace wmoge
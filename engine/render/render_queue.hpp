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

#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "core/pool_vector.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"

#include <array>
#include <cinttypes>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class RenderCmdKey
     * @brief Key to sort commands for efficient execution
     */
    struct RenderCmdKey final {
        std::uint64_t value = 0;

        bool operator<(const RenderCmdKey& other) const { return value < other.value; }
        bool operator==(const RenderCmdKey& other) const { return value == other.value; }
        bool operator!=(const RenderCmdKey& other) const { return value != other.value; }
    };

    static_assert(std::is_trivially_destructible_v<RenderCmdKey>, "render cmd key must be trivial as possible");
    static_assert(sizeof(RenderCmdKey) == sizeof(std::uint64_t), "render cmd key must fit 8 bytes");

    /**
     * @class RenderCmd
     * @brief POD-Command representing single draw call
     * 
     * Low-level render cmd, which can be submitted for Gfx driver for rendering.
     * Stores minimum required info. Sorting and merging is done outside.
     */
    struct RenderCmd final {
        static constexpr int NUM_DESC_SETS = 3;

        GfxVertBuffersSetup vert_buffers;                                       //< Source of vertex data
        GfxIndexBufferSetup index_setup;                                        //< Setup of index data for indexed draw
        GfxDescSet*         desc_sets[NUM_DESC_SETS]{nullptr, nullptr, nullptr};//< Custom desc sets with frame, material and mesh data
        int                 desc_sets_slots[NUM_DESC_SETS]{-1, -1, -1};         //< Custom desc sets slots
        GfxPipeline*        pipeline = nullptr;                                 //< Complete compiled PSO to render
        GfxDrawCall         call_params;                                        //< Params to do a draw
        int                 primitive_buffer = -1;                              //< Idx of vert buffer to put dynamic instancing data
    };

    static_assert(std::is_trivially_destructible_v<RenderCmd>, "render cmd must be trivial as possible");
    static_assert(sizeof(RenderCmd) <= 128, "render cmd must fit 128 bytes");

    /**
     * @class SortableRenderCmd
     * @brief POD-Command used for sorting and draw calls merging
     * 
     * Captures low-level cmd, sorting options and instaning options.
     * When sorted, groupds cmds together for efficient merging.
     */
    struct SortableRenderCmd final {
        RenderCmd*   cmd          = nullptr;       //< Actual cmd for gfx submission, ref by ptr for raster sorting
        RenderCmdKey cmd_key      = RenderCmdKey();//< Cmd specifics for sorting (pso, material)
        int          bucket_slot  = -1;            //< Optional index for dynamic instancing for draw calls merging (if suppported)
        int          primitive_id = -1;            //< Primitive id for data fetch on gpu

        bool operator<(const SortableRenderCmd& other) const {
            if (cmd_key != other.cmd_key) {
                return cmd_key < other.cmd_key;
            }
            if (bucket_slot != other.bucket_slot) {
                return bucket_slot < other.bucket_slot;
            }
            return false;
        }
    };

    static_assert(std::is_trivially_destructible_v<SortableRenderCmd>, "sortable render cmd must be trivial as possible");
    static_assert(sizeof(SortableRenderCmd) <= 24, "sortable render cmd must fit 24 bytes");

    /**
     * @class RenderCmdAllocator
     * @brief Thread-safe allocator for RenderCmd withing one frame with persistent placement in memory
     * 
     * Allows to allocated render cmds globally for any mesh pass and any render camera.
     * Allocated commands have persistent ptr, so it is safe to pass it around withing frame.
     */
    class RenderCmdAllocator final {
    public:
        static constexpr int RENDER_CMD_PER_NODE = 1024;//< Cmds count in a single allocation

        RenderCmd* allocate();
        void       clear();

    private:
        PoolVector<RenderCmd, RENDER_CMD_PER_NODE> m_buffer;
        SpinMutex                                  m_mutex;
    };

    /**
     * @class RenderQueue
     * @brief Thread-safe queue to collect draw commands for gfx submission
     * 
     * Stores sortable light-weight render commands, which can be sorted fast and merged
     * into dynamically instanced draw calls.
     */
    class RenderQueue final {
    public:
        void push(const SortableRenderCmd& cmd);
        void clear();
        void sort();
        int  execute(GfxCtx* gfx_ctx) const;

        [[nodiscard]] std::vector<SortableRenderCmd>& get_queue();
        [[nodiscard]] const SortableRenderCmd&        get_cmd(std::size_t index) const;
        [[nodiscard]] std::size_t                     get_size() const;
        [[nodiscard]] bool                            is_empty() const;

    private:
        std::vector<SortableRenderCmd> m_queue;
        SpinMutex                      m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_QUEUE_HPP

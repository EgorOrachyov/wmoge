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

#ifndef WMOGE_DRAW_QUEUE_HPP
#define WMOGE_DRAW_QUEUE_HPP

#include "render/draw_cmd.hpp"

#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class DrawCmdSortingKey
     * @brief Small sorting key used to order all cmds before rendering
     */
    struct DrawCmdSortingKey {
        std::uint64_t value = 0;

        static DrawCmdSortingKey make_overlay(RenderMaterial* material, int layer_id);
    };

    static_assert(sizeof(DrawCmdSortingKey) == 8, "key must fit 8 bytes");

    /**
     * @class DrawCmdQueue
     * @brief Thread-safe queue to submit and sort draw commands for rendering
     *
     * Thread-safe queue to push commands of render scene objects for rendering.
     * Queue per `DrawPass` is stored inside each rendered view. Queue is used
     * to push compiled commands with their sorting keys for drawing. When all
     * commands collected, the queue is sorted depending on pass type and executed.
     *
     * Queue collects all commands to be drawn in advance, what allows system
     * parallel processing of commands, sorting for better GPU draw efficiency,
     * and efficient thread-safe parallel Gfx command list generation.
     *
     * @note Queue stores only pointers for commands. The lifetime of commands must
     *       be controlled externally by those who generate and submit commands.
     */
    class DrawCmdQueue {
    public:
        void push(DrawCmdSortingKey key, DrawCmd* cmd);
        void reserve(std::size_t size);
        void clear();
        void sort();
        void execute(GfxDriver* driver, DrawUniformBuffer* pass_buffers, int pass_buffers_count);

        std::vector<std::pair<DrawCmdSortingKey, DrawCmd*>>&       get_cmds();
        const std::vector<std::pair<DrawCmdSortingKey, DrawCmd*>>& get_cmds() const;

    private:
        using SortEntry = std::pair<DrawCmdSortingKey, DrawCmd*>;
        using SortList  = std::vector<SortEntry>;

        SortList   m_cmds;
        std::mutex m_mutex;
    };

    static_assert(sizeof(DrawCmdQueue) >= 64, "queue size must be large enough to better fit memory cache");

}// namespace wmoge

#endif//WMOGE_DRAW_QUEUE_HPP

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

#include <algorithm>

namespace wmoge {

    void RenderQueue::push(RenderCmdKey key, const RenderCmd& cmd) {
        std::lock_guard guard(m_mutex);

        const int index = m_next_index++;
        m_buffer[index] = cmd;
        m_queue.emplace_back(key, index);
    }
    void RenderQueue::clear() {
        std::lock_guard guard(m_mutex);

        m_next_index = 0;
        m_queue.clear();
    }
    void RenderQueue::free() {
        std::lock_guard guard(m_mutex);

        m_next_index = 0;
        m_buffer.clear();
        m_queue.clear();
    }
    void RenderQueue::sort() {
        std::lock_guard guard(m_mutex);

        std::sort(m_queue.begin(), m_queue.end(), [](const auto& p1, const auto& p2) -> bool {
            return p1.first.value < p2.first.value;
        });
    }

    std::size_t RenderQueue::size() const {
        return m_next_index;
    }
    const RenderCmd& RenderQueue::cmd(std::size_t index) const {
        assert(index < m_next_index);
        return m_buffer[m_queue[index].second];
    }

}// namespace wmoge
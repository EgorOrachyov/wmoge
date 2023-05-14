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

#include "cmd_stream.hpp"

#include "debug/profiler.hpp"

#include <cassert>

namespace wmoge {

    bool CmdStream::consume() {
        std::function<void()> cmd;
        {
            WG_AUTO_PROFILE_CORE("CmdStream::wait_for_cmd");

            std::unique_lock lock(m_mutex);
            m_cv.wait(lock, [&]() { return !m_queue.empty() || m_is_closed.load(); });

            if (m_queue.empty() || m_is_closed.load()) {
                return false;
            }

            cmd = std::move(m_queue.front());
            m_queue.pop();
        }

        assert(cmd);

        cmd();
        return true;
    }

    void CmdStream::wait() {
        WG_AUTO_PROFILE_CORE("CmdStream::wait");

        std::atomic_bool marker{false};

        push([&]() {
            marker.store(true);
            m_cv.notify_all();
        });

        std::unique_lock lock(m_mutex);
        m_cv.wait(lock, [&]() { return marker.load(); });
    }

    void CmdStream::push_close() {
        WG_AUTO_PROFILE_CORE("CmdStream::push_close");

        std::lock_guard guard(m_mutex);
        m_is_closed.store(true);
        m_cv.notify_all();
    }

}// namespace wmoge
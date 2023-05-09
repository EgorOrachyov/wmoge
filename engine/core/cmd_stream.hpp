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

#ifndef WMOGE_CMD_STREAM_HPP
#define WMOGE_CMD_STREAM_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

namespace wmoge {

    /**
     * @class CmdStream
     * @brief Thread-safe unbound multiple-producers single consumer cmd stream
     */
    class CmdStream {
    public:
        CmdStream()            = default;
        CmdStream(CmdStream&)  = delete;
        CmdStream(CmdStream&&) = delete;
        ~CmdStream()           = default;

        /** @brief Consumes a single command and returns true on success */
        bool consume();

        /** @brief Blocks execution until all submitted commands at this moment are processed */
        void wait();

        /** @brief Push close for a consumer thread */
        void push_close();

        /**
         * @brief Push callback to be consumed
         *
         * @tparam Callable Type of object which can be called without arguments
         * @param callable Object to be called on consume
         */
        template<typename Callable>
        void push(Callable&& callable);

        /**
         * @brief Push callback to be consumed and wait until consumed
         *
         * @tparam Callable Type of object which can be called without arguments
         * @param callable Object to be called on consume
         */
        template<typename Callable>
        void push_and_wait(Callable&& callable);

    private:
        std::queue<std::function<void()>> m_queue;
        std::mutex                        m_mutex;
        std::condition_variable           m_cv;
        std::atomic_bool                  m_is_closed{false};
    };

    template<typename Callable>
    void CmdStream::push(Callable&& callable) {
        std::lock_guard guard(m_mutex);
        m_queue.emplace(std::forward<Callable>(callable));
        m_cv.notify_all();
    }

    template<typename Callable>
    void CmdStream::push_and_wait(Callable&& callable) {
        push(std::forward<Callable>(callable));
        wait();
    }

}// namespace wmoge

#endif//WMOGE_CMD_STREAM_HPP

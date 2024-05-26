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

#include "core/synchronization.hpp"

#include <atomic>
#include <functional>
#include <queue>

namespace wmoge {

    /**
     * @class CallbackStream
     * @brief Thread-safe unbound multiple-producers single consumer callback stream
     */
    class CallbackStream {
    public:
        CallbackStream()                 = default;
        CallbackStream(CallbackStream&)  = delete;
        CallbackStream(CallbackStream&&) = delete;
        ~CallbackStream()                = default;

        void set_consumer_id(std::thread::id thread_id);

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
        std::atomic_bool                  m_is_closed{false};
        std::thread::id                   m_thread_id;
        SpinMutex                         m_mutex;
    };

    template<typename Callable>
    void CallbackStream::push(Callable&& callable) {
        if (std::this_thread::get_id() == m_thread_id) {
            callable();
            return;
        }

        std::lock_guard guard(m_mutex);
        m_queue.emplace(std::forward<Callable>(callable));
    }

    template<typename Callable>
    void CallbackStream::push_and_wait(Callable&& callable) {
        push(std::forward<Callable>(callable));
        wait();
    }

}// namespace wmoge
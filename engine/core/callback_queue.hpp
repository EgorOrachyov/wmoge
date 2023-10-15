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

#ifndef WMOGE_CALLBACK_QUEUE_HPP
#define WMOGE_CALLBACK_QUEUE_HPP

#include "core/synchronization.hpp"

#include <cstddef>
#include <functional>
#include <vector>

namespace wmoge {

    /**
     * @class CallbackQueue
     * @brief Thread-safe callback queue to schedule callbacks on a particular thread
     */
    class CallbackQueue {
    public:
        CallbackQueue()                = default;
        CallbackQueue(CallbackQueue&)  = delete;
        CallbackQueue(CallbackQueue&&) = delete;
        ~CallbackQueue()               = default;

        /**
         * @brief Push callback to call later in a queue
         * @param callback Callback to call
         */
        template<class Callable>
        void push(Callable&& callback);

        /**
         * @brief Flush queue and executes pending callbacks
         * @note Must be called on a target thread for the execution
         */
        void flush();

        /** @brief Remove all commands without invocation */
        void clear();

    private:
        std::vector<std::function<void()>> m_queue;
        SpinMutex                          m_mutex;
    };

    template<class Callable>
    void CallbackQueue::push(Callable&& callback) {
        std::lock_guard guard(m_mutex);
        m_queue.emplace_back(std::forward<Callable>(callback));
    }

}// namespace wmoge

#endif//WMOGE_CALLBACK_QUEUE_HPP

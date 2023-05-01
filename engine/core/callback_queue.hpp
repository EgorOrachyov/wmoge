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

#include <cstddef>
#include <functional>
#include <mutex>
#include <vector>

#include "core/log.hpp"

namespace wmoge {

    /**
     * @class CallbackQueue
     * @brief Thread-safe callback queue to schedule callbacks on a particular thread
     */
    class CallbackQueue {
    public:
        /** Default buffer for commands allocation (1 MB) can increase later */
        static const std::size_t DEFAULT_BUFFER_SIZE = 1024 * 1024;

        explicit CallbackQueue(std::size_t buffer_size = DEFAULT_BUFFER_SIZE);
        ~CallbackQueue();

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
        struct Callback {
            virtual ~Callback()           = default;
            virtual void        execute() = 0;
            virtual std::size_t size()    = 0;
        };

        std::vector<std::uint8_t> m_memory_buffer;
        std::size_t               m_allocated_callbacks = 0;
        std::size_t               m_allocated_bytes     = 0;

        mutable std::mutex m_mutex;
    };

    template<class Callable>
    void CallbackQueue::push(Callable&& call) {
        struct CallbackWrapper final : public Callback {
            Callable c;
            explicit CallbackWrapper(Callable&& call) : c(std::forward<Callable>(call)) {}
            ~CallbackWrapper() override = default;
            void        execute() override { c(); }
            std::size_t size() override { return sizeof(CallbackWrapper); }
        };

        std::lock_guard guard(m_mutex);
        std::size_t     wrapper_size = sizeof(CallbackWrapper);

        if (wrapper_size + m_allocated_bytes <= m_memory_buffer.size()) {
            new (m_memory_buffer.data() + m_allocated_bytes) CallbackWrapper(std::forward<Callable>(call));
            m_allocated_callbacks += 1;
            m_allocated_bytes += wrapper_size;
            return;
        }

        WG_LOG_ERROR("not enough space to allocate command");
    }

}// namespace wmoge

#endif//WMOGE_CALLBACK_QUEUE_HPP

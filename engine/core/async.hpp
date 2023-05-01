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

#ifndef WMOGE_ASYNC_HPP
#define WMOGE_ASYNC_HPP

#include "core/fast_vector.hpp"
#include "core/ref.hpp"

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>

namespace wmoge {

    /**
     * @class AsyncStatus
     * @brief Status of an async operation
     */
    enum class AsyncStatus {
        Default,
        InProcess,
        Ok,
        Failed,
    };

    /**
     * @class AsyncCallback
     * @brief Callback function which optionally may be called on async op completion
     *
     * @tparam T Type of the result
     */
    template<typename T>
    using AsyncCallback = std::function<void(AsyncStatus, std::optional<T>&)>;

    /**
     * @class AsyncStateBase
     * @brief Base class for any object which want to listen to async result
     */
    class AsyncStateBase : public RefCnt {
    public:
        ~AsyncStateBase() override = default;
        virtual void notify(AsyncStatus status, AsyncStateBase* invoker) {}
    };

    /**
     * @class AsyncState
     * @brief An async operation shared data state
     *
     * @tparam T Type of the operation result
     */
    template<typename T>
    class AsyncState : public AsyncStateBase {
    public:
        ~AsyncState() override = default;

        virtual void set_in_progress() {
            assert(m_status.load() == AsyncStatus::Default);
            m_status.store(AsyncStatus::InProcess);
        }
        virtual void add_on_completion(AsyncCallback<T> callback) {
            if (!callback) return;
            std::lock_guard<std::mutex> lock(m_mutex);
            if (is_completed()) {
                callback(m_status.load(), m_result);
                return;
            }
            m_callbacks.push_back(std::move(callback));
        }
        virtual void set_failed() {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_status.store(AsyncStatus::Failed);
            m_cv.notify_all();

            fast_vector<AsyncCallback<T>> callbacks = std::move(m_callbacks);
            for (auto& c : callbacks) c(AsyncStatus::Failed, m_result);
            fast_vector<ref_ptr<AsyncStateBase>> deps = std::move(m_children);
            for (auto& d : deps) d->notify(AsyncStatus::Failed, this);
        }
        virtual void set_result(T&& result) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_result = std::make_optional(std::forward<T>(result));
            m_status.store(AsyncStatus::Ok);
            m_cv.notify_all();

            fast_vector<AsyncCallback<T>> callbacks = std::move(m_callbacks);
            for (auto& c : callbacks) c(AsyncStatus::Ok, m_result);
            fast_vector<ref_ptr<AsyncStateBase>> deps = std::move(m_children);
            for (auto& d : deps) d->notify(AsyncStatus::Ok, this);
        }
        virtual void add_dependency(ref_ptr<AsyncStateBase> dep) {
            assert(dep);
            std::lock_guard<std::mutex> lock(m_mutex);
            if (is_completed()) {
                dep->notify(status(), this);
                return;
            }
            m_children.push_back(dep);
        }

        virtual void wait_completed() {
            if (is_completed()) return;
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [&]() { return is_completed(); });
        }

        AsyncStatus status() {
            return m_status.load();
        }

        T& result() {
            std::lock_guard<std::mutex> lock(m_mutex);
            assert(m_result.has_value());
            return m_result.value();
        }

        bool is_completed() {
            auto s = status();
            return s != AsyncStatus::InProcess && s != AsyncStatus::Default;
        }
        bool is_failed() {
            return status() == AsyncStatus::Failed;
        }
        bool is_ok() {
            return status() == AsyncStatus::Ok;
        }

    protected:
        fast_vector<AsyncCallback<T>>        m_callbacks;
        fast_vector<ref_ptr<AsyncStateBase>> m_children;
        std::optional<T>                     m_result;
        std::atomic<AsyncStatus>             m_status{AsyncStatus::Default};
        std::mutex                           m_mutex;
        std::condition_variable              m_cv;
    };

    /**
     * @class AsyncOp
     * @brief Handle to control state of async operation
     *
     * @tparam T Type of the operation result
     */
    template<typename T>
    using AsyncOp = ref_ptr<AsyncState<T>>;

    /**
     * @brief Makes new async operation control state
     *
     * @tparam T Type of the operation result
     *
     * @return Created AsyncOp
     */
    template<typename T>
    AsyncOp<T> make_async_op() { return make_ref<AsyncState<T>>(); }

    /**
     * @class Async
     * @brief Handle to an asynchronous operation
     *
     * @tparam T Type of the operation result
     */
    template<typename T>
    class Async final {
    public:
        Async() = default;
        Async(ref_ptr<AsyncState<T>> op) : m_op(std::move(op)) {}

        AsyncStatus status() {
            assert(m_op);
            return m_op->status();
        }
        T& result() {
            assert(m_op);
            return m_op->result();
        }
        void wait_completed() {
            assert(m_op);
            m_op->wait_completed();
        }

        void reset() {
            m_op.reset();
        }

        bool is_null() {
            return !m_op.get();
        }
        bool is_not_null() {
            return m_op.get();
        }
        bool is_completed() {
            assert(m_op);
            return m_op->is_completed();
        }
        bool is_failed() {
            assert(m_op);
            return m_op->is_failed();
        }
        bool is_ok() {
            assert(m_op);
            return m_op->is_ok();
        }

        void add_dependency(ref_ptr<AsyncStateBase> dep) {
            assert(m_op);
            m_op->add_dependency(std::move(dep));
        }

    private:
        ref_ptr<AsyncState<T>> m_op;
    };

}// namespace wmoge

#endif//WMOGE_ASYNC_HPP

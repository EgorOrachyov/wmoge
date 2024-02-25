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

#include "core/array_view.hpp"
#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "core/synchronization.hpp"

#include <cassert>
#include <functional>
#include <optional>

namespace wmoge {

    /**
     * @brief Status of an async operation
     */
    enum class AsyncStatus {
        InProcess,
        Ok,
        Failed,
    };

    /**
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
        virtual void        notify(AsyncStatus status, AsyncStateBase* invoker) {}
        virtual void        add_dependency(const Ref<AsyncStateBase>& dependency) = 0;
        virtual void        wait_completed()                                      = 0;
        virtual bool        is_completed()                                        = 0;
        virtual bool        is_failed()                                           = 0;
        virtual bool        is_ok()                                               = 0;
        virtual AsyncStatus status()                                              = 0;
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

        virtual void add_on_completion(AsyncCallback<T> callback) {
            if (!callback) return;
            std::lock_guard lock(m_mutex);
            if (is_completed()) {
                callback(m_status.load(), m_result);
                return;
            }
            m_callbacks.push_back(std::move(callback));
        }
        virtual void set_failed() {
            std::lock_guard lock(m_mutex);
            m_status.store(AsyncStatus::Failed);

            fast_vector<AsyncCallback<T>> callbacks = std::move(m_callbacks);
            for (auto& c : callbacks) c(AsyncStatus::Failed, m_result);
            fast_vector<Ref<AsyncStateBase>> deps = std::move(m_children);
            for (auto& d : deps) d->notify(AsyncStatus::Failed, this);
        }
        virtual void set_result(T&& result) {
            std::lock_guard lock(m_mutex);
            m_result = std::make_optional(std::forward<T>(result));
            m_status.store(AsyncStatus::Ok);

            fast_vector<AsyncCallback<T>> callbacks = std::move(m_callbacks);
            for (auto& c : callbacks) c(AsyncStatus::Ok, m_result);
            fast_vector<Ref<AsyncStateBase>> deps = std::move(m_children);
            for (auto& d : deps) d->notify(AsyncStatus::Ok, this);
        }

        void add_dependency(const Ref<AsyncStateBase>& dependency) override {
            assert(dependency);
            std::lock_guard lock(m_mutex);
            if (is_completed()) {
                dependency->notify(status(), this);
                return;
            }
            m_children.push_back(dependency);
        }

        void wait_completed() override {
            while (!is_completed()) {}
        }

        bool is_completed() override {
            return status() != AsyncStatus::InProcess;
        }
        bool is_failed() override {
            return status() == AsyncStatus::Failed;
        }
        bool is_ok() override {
            return status() == AsyncStatus::Ok;
        }

        AsyncStatus status() override {
            return m_status.load();
        }

        T& result() {
            std::lock_guard lock(m_mutex);
            assert(m_result.has_value());
            return m_result.value();
        }

        std::optional<T> result_opt() {
            std::lock_guard lock(m_mutex);
            return m_result;
        }

    protected:
        fast_vector<AsyncCallback<T>>    m_callbacks;
        fast_vector<Ref<AsyncStateBase>> m_children;
        std::optional<T>                 m_result;
        std::atomic<AsyncStatus>         m_status{AsyncStatus::InProcess};
        SpinMutex                        m_mutex;
    };

    /**
     * @brief Handle to control state of async operation
     *
     * @tparam T Type of the operation result
     */
    template<typename T>
    using AsyncOp = Ref<AsyncState<T>>;

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
     * @class AsyncBase
     * @brief Base class for tracker for an async operation
     *
     * @tparam State Type of the internal state to track
     */
    template<typename State>
    class AsyncBase {
    public:
        AsyncBase() = default;
        explicit AsyncBase(Ref<State> state) : m_state(std::move(state)) {}

        AsyncStatus status() {
            assert(m_state);
            return m_state->status();
        }
        void wait_completed() {
            assert(m_state);
            m_state->wait_completed();
        }

        void reset() {
            m_state.reset();
        }

        bool is_null() {
            return !m_state.get();
        }
        bool is_not_null() {
            return m_state.get();
        }
        bool is_completed() {
            assert(m_state);
            return m_state->is_completed();
        }
        bool is_failed() {
            assert(m_state);
            return m_state->is_failed();
        }
        bool is_ok() {
            assert(m_state);
            return m_state->is_ok();
        }

        void add_dependency(const Ref<AsyncStateBase>& dependency) {
            assert(m_state);
            m_state->add_dependency(dependency);
        }

    protected:
        Ref<State> m_state;
    };

    /**
     * @class Async
     * @brief Handle to an asynchronous operation (may have result or may not)
     *
     * @tparam T Type of the operation result
     */
    class Async final : public AsyncBase<AsyncStateBase> {
    public:
        Async() = default;
        explicit Async(Ref<AsyncStateBase> state) : AsyncBase<AsyncStateBase>(std::move(state)) {}

        /**
         * @brief Make compound async to join all other async operations
         *
         * @param dependencies List of async operations to join making new async
         *
         * @return New async signaled when all deps finished or failed if one failed
         */
        static Async join(ArrayView<Async> dependencies);
    };

    /**
     * @class AsyncResult
     * @brief Handle to an asynchronous operation with a result to get
     *
     * @tparam T Type of the operation result
     */
    template<typename T>
    class AsyncResult final : public AsyncBase<AsyncState<T>> {
    public:
        using AsyncBase<AsyncState<T>>::m_state;

        AsyncResult() = default;
        explicit AsyncResult(Ref<AsyncState<T>> state) : AsyncBase<AsyncState<T>>(std::move(state)) {}

        void add_on_completion(AsyncCallback<T> callback) {
            assert(m_state);
            m_state->add_on_completion(std::move(callback));
        }

        T& result() {
            assert(m_state);
            return m_state->result();
        }

        std::optional<T> result_opt() {
            assert(m_state);
            return m_state->result_opt();
        }

        Async as_async() {
            assert(m_state);
            return Async(m_state.template as<AsyncStateBase>());
        }
    };

}// namespace wmoge
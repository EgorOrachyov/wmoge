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

#include "core/async.hpp"

#include "async.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Async Async::join(array_view<Async> dependencies) {
        WG_PROFILE_CPU_CORE("Async::join");

        if (dependencies.empty()) {
            auto state = make_ref<AsyncState<int>>();
            state->set_result(0);
            return Async(std::move(state));
        }

        class AsyncStateJoin : public AsyncState<int> {
        public:
            explicit AsyncStateJoin(int to_wait) {
                m_deps_to_wait = to_wait;
            }

            void notify(AsyncStatus status, AsyncStateBase* invoker) override {
                WG_PROFILE_CPU_CORE("AsyncStateJoin::notify");

                assert(m_deps_to_wait > 0);

                if (status == AsyncStatus::Ok) {
                    assert(m_deps_ok.load() < m_deps_to_wait);
                    bool do_ok = (m_deps_ok.fetch_add(1) == m_deps_to_wait - 1);

                    if (do_ok) {
                        set_result(0);
                    }
                }

                if (status == AsyncStatus::Failed) {
                    assert(m_deps_failed.load() < m_deps_to_wait);
                    bool do_fail = (m_deps_failed.fetch_add(1) == 0);

                    if (do_fail) {
                        set_failed();
                    }
                }
            }

        private:
            int             m_deps_to_wait = 0;
            std::atomic_int m_deps_ok{0};
            std::atomic_int m_deps_failed{0};
        };

        auto state      = make_ref<AsyncStateJoin>(int(dependencies.size()));
        auto state_base = state.as<AsyncStateBase>();

        for (auto& dependency : dependencies) {
            dependency.add_dependency(state_base);
        }

        return Async(std::move(state));
    }

    Async Async::completed() {
        auto state = make_async_op<int>();
        state->set_result(0);
        return Async(state);
    }

    Async Async::failed() {
        auto state = make_async_op<int>();
        state->set_failed();
        return Async(state);
    }

}// namespace wmoge
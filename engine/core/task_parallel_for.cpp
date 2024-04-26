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

#include "task_parallel_for.hpp"

#include "core/task_runtime.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    TaskParallelFor::TaskParallelFor(Strid name, TaskRunnableFor runnable)
        : m_runnable(std::move(runnable)),
          m_task_manager(Engine::instance()->task_manager()),
          m_name(name) {
    }

    TaskHnd TaskParallelFor::schedule(int num_elements, int batch_size) {
        return schedule(num_elements, batch_size, Async{});
    }

    TaskHnd TaskParallelFor::schedule(int num_elements, int batch_size, Async depends_on) {
        WG_AUTO_PROFILE_CORE("TaskParallelFor::schedule");

        assert(m_runnable);
        assert(m_task_manager);
        assert(batch_size > 0);

        if (num_elements <= 0) {
            auto state = make_ref<AsyncState<int>>();
            state->set_result(0);
            return TaskHnd(state);
        }

        auto parallel_for_runtime = make_ref<TaskRuntimeParallelFor>(m_name, m_runnable, m_task_manager, num_elements, batch_size);

        if (depends_on.is_not_null()) {
            depends_on.add_dependency(parallel_for_runtime);
        } else {
            parallel_for_runtime->submit();
        }

        return TaskHnd(parallel_for_runtime);
    }

}// namespace wmoge
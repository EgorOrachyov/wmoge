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

#include "core/task.hpp"

#include "core/ioc_container.hpp"
#include "core/task_runtime.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Task::Task(Strid name, TaskRunnable runnable)
        : m_runnable(std::move(runnable)),
          m_name(name) {
    }

    TaskHnd Task::schedule(TaskManager* task_manager) {
        return schedule(task_manager, Async{});
    }

    TaskHnd Task::schedule(TaskManager* task_manager, Async depends_on) {
        WG_PROFILE_CPU_CORE("Task::schedule");

        assert(m_runnable);
        assert(task_manager);

        auto runtime = make_ref<TaskRuntime>(m_name, m_runnable, task_manager);

        if (depends_on.is_not_null()) {
            depends_on.add_dependency(runtime.as<AsyncStateBase>());
        } else {
            runtime->submit();
        }

        return TaskHnd(runtime);
    }

}// namespace wmoge

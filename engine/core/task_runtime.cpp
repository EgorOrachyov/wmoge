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

#include "task_runtime.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "math/math_utils.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    TaskRuntime::TaskRuntime(Strid name, TaskRunnable runnable, TaskManager* task_manager) {
        assert(runnable);
        assert(task_manager);

        m_name         = name;
        m_runnable     = std::move(runnable);
        m_task_manager = task_manager;
    }

    void TaskRuntime::notify(AsyncStatus status, AsyncStateBase* invoker) {
        if (status == AsyncStatus::Ok) {
            assert(TaskRuntime::status() == AsyncStatus::InProcess);
            submit();
        }
        if (status == AsyncStatus::Failed) {
            assert(TaskRuntime::status() == AsyncStatus::InProcess);
            set_failed();

            WG_LOG_ERROR("failed: " << m_name << " dep failed");
        }
    }

    void TaskRuntime::wait_completed() {
        WG_AUTO_PROFILE_TASK("TaskRuntime::wait_completed", m_name.str());
        AsyncState::wait_completed();
    }

    void TaskRuntime::submit() {
        WG_AUTO_PROFILE_TASK("TaskRuntime::submit", m_name.str());
        m_task_manager->submit(Ref<TaskRuntime>(this));
    }

    void TaskRuntime::execute(TaskContext& context) {
        WG_AUTO_PROFILE_TASK("TaskRuntime::execute", m_name.str());

        auto ret = m_runnable(context);

        if (ret) {
            set_failed();
            WG_LOG_ERROR("failed: " << m_name << " worker: " << context.thread_name());
        } else {
            set_result(0);
        }
    }

    TaskRuntimeParallelFor::TaskRuntimeParallelFor(Strid name, TaskRunnableFor runnable, TaskManager* task_manager, int num_elements, int batch_size) {
        assert(runnable);
        assert(task_manager);
        assert(num_elements > 0);
        assert(batch_size > 0);

        m_name         = name;
        m_runnable     = std::move(runnable);
        m_task_manager = task_manager;
        m_num_elements = num_elements;
        m_batch_size   = batch_size;

        int num_batches = Math::div_up(m_num_elements, m_batch_size);
        m_num_tasks     = Math::min(num_batches, m_task_manager->get_num_workers());
    }

    void TaskRuntimeParallelFor::notify(AsyncStatus status, AsyncStateBase* invoker) {
        if (status == AsyncStatus::Ok) {
            assert(TaskRuntimeParallelFor::status() == AsyncStatus::InProcess);
            submit();
        }
        if (status == AsyncStatus::Failed) {
            assert(TaskRuntimeParallelFor::status() == AsyncStatus::InProcess);
            set_failed();

            WG_LOG_ERROR("failed: " << m_name << " dep failed");
        }
    }

    void TaskRuntimeParallelFor::wait_completed() {
        WG_AUTO_PROFILE_TASK("TaskRuntimeParallelFor::wait_completed", m_name.str());
        AsyncState::wait_completed();
    }

    void TaskRuntimeParallelFor::submit() {
        WG_AUTO_PROFILE_TASK("TaskRuntimeParallelFor::submit", m_name.str());

        Ref<TaskRuntimeParallelFor> shared_state(this);

        TaskRunnable runnable_function = [shared_state](TaskContext& context) -> int {
            int batch_size   = shared_state->m_batch_size;
            int num_elements = shared_state->m_num_elements;

            int item_start = shared_state->m_item_allocator.fetch_add(batch_size);
            int item_end   = item_start + batch_size;

            bool has_error = false;

            while (item_start < num_elements) {
                for (int item_id = item_start; item_id < num_elements && item_id < item_end; item_id += 1) {
                    has_error = has_error || shared_state->m_runnable(context, item_id, num_elements) != 0;
                }

                item_start = shared_state->m_item_allocator.fetch_add(batch_size);
                item_end   = item_start + batch_size;
            }

            if (has_error) {
                int tasks_failed = shared_state->m_tasks_failed.fetch_add(1);
                if (tasks_failed == 0) shared_state->set_failed();
            }

            if (!has_error) {
                int tasks_finished = shared_state->m_tasks_finished.fetch_add(1);
                if (tasks_finished + 1 == shared_state->m_num_tasks) shared_state->set_result(0);
            }

            return 0;
        };

        for (int i = 0; i < m_num_tasks; i++) {
            auto job_name = SID(m_name.str() + "-" + StringUtils::from_int(i));
            auto job      = make_ref<TaskRuntime>(job_name, runnable_function, m_task_manager);
            job->submit();
        }
    }

}// namespace wmoge

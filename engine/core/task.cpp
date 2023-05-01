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
#include "core/engine.hpp"
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    Task::Task(StringId name) : m_name(name) {
        m_runnable = [](TaskContext&) { return 0; };
    }
    Task::Task(TaskRunnable runnable) : Task(StringId(), std::move(runnable)) {
    }
    Task::Task(StringId name, TaskRunnable runnable) : m_runnable(std::move(runnable)), m_name(name) {
    }

    void Task::set_task_manager(class TaskManager* task_manager) {
        assert(!m_run_called.load());
        m_task_manager = task_manager;
    }

    void Task::add_to_wait(const ref_ptr<class Task>& task) {
        assert(task);
        assert(!m_run_called.load());
        {
            std::lock_guard guard(m_mutex);
            m_to_wait_total += 1;
        }
        task->add_dependency(ref_ptr<AsyncStateBase>(this));
    }

    void Task::add_to_notify(const ref_ptr<class Task>& task) {
        assert(task);
        task->add_to_wait(ref_ptr<Task>(this));
    }

    TaskAsync Task::run() {
        std::lock_guard guard(m_mutex);

        m_run_called.store(true);

        if (m_to_wait_total == 0 || m_to_wait_total == m_ok) {
            submit();
        }

        return TaskAsync(ref_ptr<Task>(this));
    }

    void Task::execute(TaskContext& context) {
        WG_AUTO_PROFILE_TASK(m_name.str());

        auto ret = m_runnable(context);

        if (ret) {
            set_failed();
            WG_LOG_ERROR("failed: " << m_name << " worker: " << context.thread_name());
        } else {
            set_result(0);
        }
    }
    void Task::submit() {
        TaskManager* task_manager = m_task_manager ? m_task_manager : Engine::instance()->task_manager();
        task_manager->submit(ref_ptr<Task>(this));
    }

    void Task::notify(AsyncStatus status, AsyncStateBase* invoker) {
        bool can_run  = false;
        bool can_fail = false;
        {
            std::lock_guard guard(m_mutex);
            assert(m_to_wait_total > 0);

            if (status == AsyncStatus::Ok) {
                m_ok += 1;
                can_run = (m_ok == m_to_wait_total) && m_run_called.load();
            }
            if (status == AsyncStatus::Failed) {
                m_failed += 1;
                can_fail = (m_failed == 1);
            }
        }

        if (can_run) {
            submit();
        }

        if (can_fail) {
            set_failed();
            WG_LOG_ERROR("failed: " << m_name << " dep failed");
        }
    }
    void Task::wait_completed() {
        WG_AUTO_PROFILE_TASK(m_name.str());
        AsyncState::wait_completed();
    }

}// namespace wmoge

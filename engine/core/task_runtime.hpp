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

#include "core/async.hpp"
#include "core/string_id.hpp"
#include "core/task.hpp"
#include "core/task_manager.hpp"
#include "core/task_parallel_for.hpp"

#include <atomic>
#include <functional>

namespace wmoge {

    /**
     * @class TaskRuntime
     * @brief State of scheduled running task
     */
    class TaskRuntime final : public AsyncState<int> {
    public:
        TaskRuntime(StringId name, TaskRunnable runnable, TaskManager* task_manager);

        void notify(AsyncStatus status, AsyncStateBase* invoker) override;
        void wait_completed() override;
        void submit();
        void execute(TaskContext& context);

    private:
        TaskRunnable m_runnable;
        TaskManager* m_task_manager;
        StringId     m_name;
    };

    /**
     * @class TaskRuntimeParallelFor
     * @brief State of scheduled running parallel-for task
     */
    class TaskRuntimeParallelFor final : public AsyncState<int> {
    public:
        TaskRuntimeParallelFor(StringId name, TaskRunnableFor runnable, TaskManager* task_manager, int num_elements, int batch_size);

        void notify(AsyncStatus status, AsyncStateBase* invoker) override;
        void wait_completed() override;
        void submit();

    private:
        TaskRunnableFor m_runnable;
        TaskManager*    m_task_manager;
        StringId        m_name;
        int             m_num_elements;
        int             m_batch_size;
        int             m_num_tasks;
        std::atomic_int m_item_allocator{0};
        std::atomic_int m_tasks_finished{0};
        std::atomic_int m_tasks_failed{0};
    };

}// namespace wmoge
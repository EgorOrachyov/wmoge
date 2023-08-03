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

#ifndef WMOGE_TASK_PARALLEL_FOR_HPP
#define WMOGE_TASK_PARALLEL_FOR_HPP

#include "core/async.hpp"
#include "core/string_id.hpp"
#include "core/task_hnd.hpp"
#include "core/task_manager.hpp"

#include <functional>

namespace wmoge {

    /**
     * @brief Task runnable used to make task for
     *
     * Accepts context of execution, item id and total items count.
     */
    using TaskRunnableFor = std::function<int(TaskContext&, int, int)>;

    /**
     * @class TaskParallelFor
     * @brief Some parallel-for job to operate on a set of elements
     *
     * This is special type of a task. In fact, it may spawn a set of jobs to be
     * executed in a task manager. This jobs will share elements to operate, using
     * work-stealing principle. Each job is responsible for a batch of a work.
     *
     * Typical use cases:
     *  - Run a parallel job for a set of equal objects
     *  - Background tasks using worker threads for a heavy objects array processing
     *
     *  How to use:
     *  - Create task, specifying runnable function, name and manager
     *  - Schedule task to run, specifying number of objects and batch size
     */
    class TaskParallelFor {
    public:
        /**
         * @brief Construct task with runnable job and with a label for debugging
         *
         * @param name Name of the task for debugging
         * @param runnable Function to execute as a task job
         */
        explicit TaskParallelFor(StringId name, TaskRunnableFor runnable);

        /**
         * @brief Kick off task for execution
         *
         * The task will be scheduled for background execution in selected task manager.
         *
         * @param num_elements Total number of elements to process
         * @param batch_size Size of a batch, elements to be processed by a single job
         *
         * @return Async task hnd to track task execution
         */
        TaskHnd schedule(int num_elements, int batch_size);

        /**
         * @brief Kick off task for execution
         *
         * Schedule parallel-for task to be executed only after and only of dependencies completed.
         * The jobs of the task will be scheduled for background execution in selected task manager.
         *
         * @param num_elements Total number of elements to process
         * @param batch_size Size of a batch, elements to be processed by a single job
         * @param depends_on Dependencies that must complete before this task actual execution.
         *                   As soon as dependencies completed, this task is executed. Use this
         *                   to construct task graphs for computations.
         *
         * @return Async task hnd to track task execution
         */
        TaskHnd schedule(int num_elements, int batch_size, Async depends_on);

        /** @brief Function called as a job of task; may be empty */
        [[nodiscard]] TaskRunnableFor& runnable() { return m_runnable; }

        /** @brief Optional task label for debugging */
        [[nodiscard]] const StringId& name() const { return m_name; }

        /** @brief Task manager to execute this task; null if used default engine manager */
        [[nodiscard]] TaskManager* task_manager() const { return m_task_manager; }

    private:
        TaskRunnableFor m_runnable;
        TaskManager*    m_task_manager = nullptr;
        StringId        m_name;
    };

}// namespace wmoge

#endif//WMOGE_TASK_PARALLEL_FOR_HPP

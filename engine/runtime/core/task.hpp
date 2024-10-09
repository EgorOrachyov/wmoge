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
#include "core/task_hnd.hpp"
#include "core/task_manager.hpp"

#include <atomic>
#include <functional>

namespace wmoge {

    /**
     * @brief Function which used to make task
     *
     * Accepts context of execution.
     */
    using TaskRunnable = std::function<int(TaskContext&)>;

    /**
     * @class Task
     * @brief Some job to be executed in task manager
     *
     * Typical use cases:
     *  - Async execution of different tasks
     *  - Background tasks using worker threads for a heavy job
     *
     *  How to use:
     *  - Create task, specifying runnable function, name and manager
     *  - Schedule task to run
     */
    class Task {
    public:
        /**
         * @brief Construct task with runnable job and with a label for debugging
         *
         * @param name Name of the task for debugging
         * @param runnable Function to execute as a task job
         */
        explicit Task(Strid name, TaskRunnable runnable);

        /**
         * @brief Kick off task for execution
         *
         * The task will be scheduled for background execution in selected task manager.
         * 
         * @param task_manager Taask manager to run task
         *  
         * @return Async task hnd to track task execution
         */
        TaskHnd schedule(TaskManager* task_manager);

        /**
         * @brief Kick off task for execution
         *
         * Schedule task to be executed only after and only of dependencies completed.
         * The task will be scheduled for background execution in selected task manager.
         *
         * @param task_manager Taask manager to run task
         * @param depends_on Dependencies that must complete before this task actual execution.
         *                   As soon as dependencies completed, this task is executed. Use this
         *                   to construct task graphs for computations.
         *
         * @return Async task hnd to track task execution
         */
        TaskHnd schedule(TaskManager* task_manager, Async depends_on);

        /** @brief Function called as a job of task; may be empty */
        [[nodiscard]] TaskRunnable& runnable() { return m_runnable; }

        /** @brief Optional task label for debugging */
        [[nodiscard]] const Strid& name() const { return m_name; }

    private:
        TaskRunnable m_runnable;
        Strid        m_name;
    };

}// namespace wmoge
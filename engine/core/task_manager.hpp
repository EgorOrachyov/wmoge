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

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/synchronization.hpp"

#include <functional>
#include <list>
#include <thread>
#include <vector>

namespace wmoge {

    /**
     * @class TaskContext
     * @brief Context passed in a time of task execution
     */
    class TaskContext {
    public:
        [[nodiscard]] const Strid& thread_name() const { return m_thread_name; }
        [[nodiscard]] int          thread_id() const { return m_thread_id; }

    private:
        friend class TaskManager;

        Strid m_thread_name;
        int   m_thread_id = -1;
    };

    /**
     * @class TaskManager
     * @brief Manager for engine async task execution
     */
    class TaskManager {
    public:
        explicit TaskManager(int workers_count);
        ~TaskManager();

        void submit(Ref<class TaskRuntime> task);
        void shutdown();
        int  get_num_workers();
        int  get_num_tasks();

    private:
        bool next_to_exec(Ref<class TaskRuntime>& task);

    private:
        std::vector<std::thread>          m_workers;
        std::list<Ref<class TaskRuntime>> m_background_queue;
        std::atomic_bool                  m_finished{false};
        SpinMutex                         m_mutex;
    };

}// namespace wmoge
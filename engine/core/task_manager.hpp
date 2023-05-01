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

#ifndef WMOGE_TASK_MANAGER_HPP
#define WMOGE_TASK_MANAGER_HPP

#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "task.hpp"

namespace wmoge {

    /**
     * @class TaskManager
     * @brief Manager for engine async task execution
     */
    class TaskManager {
    public:
        explicit TaskManager(int workers_count);
        ~TaskManager();

        void submit(ref_ptr<Task> task);
        void shutdown();

    private:
        ref_ptr<Task> next_to_exec();

    private:
        std::vector<std::thread> m_workers;
        std::list<ref_ptr<Task>> m_background_queue;
        std::atomic_bool         m_finished{false};
        std::mutex               m_mutex;
        std::condition_variable  m_cv;
    };

}// namespace wmoge

#endif//WMOGE_TASK_MANAGER_HPP

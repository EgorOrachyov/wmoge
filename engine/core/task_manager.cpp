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

#include "task_manager.hpp"
#include "core/engine.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    TaskManager::TaskManager(int workers_count) {
        assert(workers_count > 0);

        Profiler* profiler = Engine::instance()->profiler();

        for (int i = 0; i < workers_count; i++) {
            std::thread worker([this, i]() {
                TaskContext context;
                context.m_thread_name = SID("worker-" + std::to_string(i));
                context.m_thread_id   = i;

                while (!m_finished.load()) {
                    if (auto task = next_to_exec()) {
                        context.m_task = task.get();
                        task->execute(context);
                    }
                }
            });
            profiler->add_tid(worker.get_id(), SID("worker-" + std::to_string(i)));
            m_workers.push_back(std::move(worker));
        }
    }

    TaskManager::~TaskManager() {
        shutdown();
    }

    void TaskManager::submit(ref_ptr<Task> task) {
        assert(task);

        if (m_finished.load()) return;

        std::lock_guard guard(m_mutex);
        task->set_in_progress();
        TaskAsync hnd(task);
        m_background_queue.push_back(std::move(task));
        m_cv.notify_one();
    }

    void TaskManager::shutdown() {
        WG_LOG_INFO("shutdown and join already started tasks");

        m_finished.store(true);
        m_cv.notify_all();

        for (auto& worker : m_workers) {
            worker.join();
        }

        m_workers.clear();
        m_background_queue.clear();
    }

    ref_ptr<Task> TaskManager::next_to_exec() {
        std::unique_lock guard(m_mutex);
        m_cv.wait(guard, [this]() { return !m_background_queue.empty() || m_finished.load(); });

        if (m_background_queue.empty() || m_finished.load())
            return nullptr;

        auto task = m_background_queue.front();
        m_background_queue.pop_front();
        return task;
    }

}// namespace wmoge
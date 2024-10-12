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

#include "async_file_system.hpp"

#include "core/ioc_container.hpp"
#include "core/task.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    IoAsyncFileSystem::IoAsyncFileSystem(class IocContainer* ioc, int num_workers) : m_task_manager(num_workers, "async-io") {
        m_file_system = ioc->resolve_value<FileSystem>();
    }

    AsyncResult<IoAsyncFileSystem::BufferView> IoAsyncFileSystem::read_file(const std::string& filepath, BufferView buffer_view) {
        AsyncOp<BufferView> async_result = make_async_op<BufferView>();

        Task task(SID(filepath), [=](TaskContext&) -> int {
            FileOpenModeFlags mode = {FileOpenMode::In, FileOpenMode::Binary};
            Ref<File>         file;
            if (!m_file_system->open_file(filepath, file, mode)) {
                WG_LOG_ERROR("failed open file " << filepath);
                return 1;
            }
            WG_PROFILE_CPU_SCOPE_WITH_DESC(io, "IoAsyncFileSystem::read_file", filepath);
            if (!file->nread(buffer_view.data(), buffer_view.size())) {
                WG_LOG_ERROR("failed read file " << filepath);
                return 1;
            }
            return 0;
        });

        task.schedule(&m_task_manager).add_on_completion([=](AsyncStatus status, std::optional<int>&) {
            if (status == AsyncStatus::Ok) {
                async_result->set_result(BufferView(buffer_view));
                return;
            }
            async_result->set_failed();
        });

        return AsyncResult<BufferView>(async_result);
    }

}// namespace wmoge

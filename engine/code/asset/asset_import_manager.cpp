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

#include "asset_import_manager.hpp"

#include "core/ioc_container.hpp"
#include "core/task.hpp"
#include "core/task_manager.hpp"
#include "core/timer.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    AssetImportManager::AssetImportManager(UuidProvider  uuid_provider,
                                           IoContext     io_context,
                                           FileSystem*   file_system,
                                           IocContainer* ioc_containter,
                                           TaskManager*  task_manager)
        : m_uuid_provider(std::move(uuid_provider)),
          m_io_context(std::move(io_context)),
          m_file_system(file_system),
          m_ioc_containter(ioc_containter),
          m_task_manager(task_manager) {
    }

    AsyncResult<AssetImportResult> AssetImportManager::import(const std::string& path, AssetImporter* importer, const Ref<AssetImportSettings>& settings, AssetImportEnv env, const DepsResolver& deps_resolver) {
        WG_PROFILE_CPU_ASSET("AssetImportManager::import");

        std::lock_guard guard(m_mutex);

        auto query = m_importing.find(path);
        if (query != m_importing.end()) {
            return AsyncResult<AssetImportResult>(query->second->async_op);
        }

        std::filesystem::path fs_path(path);
        std::string           fs_folder = fs_path.parent_path().string();

        Ref<Entry> entry      = make_ref<Entry>();
        entry->async_op       = make_async_op<AssetImportResult>();
        entry->settings       = settings;
        entry->importer       = importer;
        entry->import_context = std::make_unique<AssetImportContext>(fs_folder, std::move(env), m_uuid_provider, m_io_context, m_file_system, m_ioc_containter);

        if (!entry->importer->collect_dependencies(*entry->import_context, path, settings)) {
            WG_LOG_ERROR("failed to collect deps to import asset at " << path);
            return AsyncResult<AssetImportResult>::failed();
        }

        for (const AssetId& dep_id : entry->import_context->get_asset_deps()) {
            entry->deps.push_back(deps_resolver(dep_id).as_async());
        }

        Task import_task(SIDDBG(path), [entry, path, this](TaskContext&) -> Status {
            Timer timer;
            timer.start();

            std::filesystem::path fs_path(path);
            std::string           fs_folder = fs_path.parent_path().string();

            if (!entry->importer->import(*entry->import_context, path, entry->settings)) {
                WG_LOG_ERROR("failed import asset at " << path);
                return StatusCode::Error;
            }

            timer.stop();
            WG_LOG_INFO("(re)import asset " << path << ", time: " << timer.get_elapsed_sec() << " sec");

            entry->async_op->set_result(std::move(entry->import_context->get_result()));
            return WG_OK;
        });

        import_task.schedule(m_task_manager, Async::join(entry->deps)).add_on_completion([entry, path, this](AsyncStatus status, std::optional<int>&) {
            if (status == AsyncStatus::Failed) {
                entry->async_op->set_failed();
            }
            std::lock_guard guard(m_mutex);
            m_importing.erase(path);
        });

        m_importing[path] = entry;
        return AsyncResult<AssetImportResult>(entry->async_op);
    }

    AsyncResult<AssetImportResult> AssetImportManager::try_find_import(const std::string& path) {
        std::lock_guard guard(m_mutex);

        auto query = m_importing.find(path);
        if (query != m_importing.end()) {
            return AsyncResult<AssetImportResult>(query->second->async_op);
        }

        return AsyncResult<AssetImportResult>();
    }

    std::optional<AssetImporter*> AssetImportManager::find_importer(const std::string& path) {
        std::lock_guard guard(m_mutex);

        std::filesystem::path fs_path(path);
        std::string           extension = fs_path.extension().string();

        if (StringUtils::is_starts_with(extension, ".")) {
            extension = extension.substr(1);
        }

        auto query = m_extensions_map.find(extension);
        if (query == m_extensions_map.end()) {
            return std::nullopt;
        }

        return query->second;
    }

    std::optional<AssetImporter*> AssetImportManager::find_importer(RttiRefClass importer_cls) {
        std::lock_guard guard(m_mutex);

        for (const auto& importer : m_importers) {
            if (importer_cls == importer->get_class()) {
                return importer.get();
            }
        }

        return std::nullopt;
    }

    void AssetImportManager::add_importer(Ref<AssetImporter> importer) {
        std::lock_guard guard(m_mutex);

        m_importers.push_back(importer);

        std::vector<std::string> extensions;
        importer->get_file_extensions(extensions);

        for (const std::string& ext : extensions) {
            m_extensions_map[ext] = importer.get();
        }
    }

}// namespace wmoge
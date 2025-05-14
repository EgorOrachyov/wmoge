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

#include "asset_artifact_cache.hpp"

#include "core/task.hpp"
#include "core/task_manager.hpp"
#include "io/async_file_system.hpp"
#include "io/stream_file.hpp"
#include "io/tree_yaml.hpp"
#include "platform/common/file_mem.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    /**
     * @class FileAssetArtifactMetaInfo
     * @brief Meta data stored along with artifact
     */
    struct FileAssetArtifactMetaInfo {
        WG_RTTI_STRUCT(FileAssetArtifactMetaInfo)

        Sha256       hash;
        std::string  name;
        std::size_t  size;
        RttiRefClass cls;
    };

    WG_RTTI_STRUCT_BEGIN(FileAssetArtifactMetaInfo) {
        WG_RTTI_FIELD(hash, {});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(size, {});
        WG_RTTI_FIELD(cls, {});
    }
    WG_RTTI_END;

    AssetArtifactCache::AssetArtifactCache(std::string        cache_path,
                                           IoContext          io_context,
                                           FileSystem*        file_system,
                                           IoAsyncFileSystem* async_file_system,
                                           TaskManager*       task_manager)
        : m_cache_path(std::move(cache_path)),
          m_io_context(std::move(io_context)),
          m_file_system(file_system),
          m_async_file_system(async_file_system),
          m_task_manager(task_manager) {
        rtti_type<FileAssetArtifactMetaInfo>();
    }

    Async AssetArtifactCache::read(UUID artifact_id, array_view<std::uint8_t> buffer, Ref<RttiObject> artifact) {
        WG_PROFILE_CPU_ASSET("AssetArtifactCache::read");

        std::lock_guard guard(m_mutex);

        auto query = m_artifacts.find(artifact_id);
        if (query == m_artifacts.end()) {
            WG_LOG_ERROR("no entry to load artifact " << artifact_id);
            return Async::failed();
        }

        if (query->second.cls != artifact->get_class()) {
            WG_LOG_ERROR("invalid artifact instance type for " << artifact_id);
            return Async::failed();
        }

        Async read_file_async = m_async_file_system->read_file(artifact_file_name(artifact_id), buffer).as_async();

        Task parse_file_task(SIDDBG("deserialize_artifact"), [this, artifact_id, buffer, artifact](TaskContext&) -> Status {
            IoContext context = m_io_context;

            Ref<FileMemReader> file = make_ref<FileMemReader>();
            WG_CHECKED(file->init(buffer));

            IoStreamFile stream;
            stream.set(file, {FileOpenMode::In, FileOpenMode::Binary});

            if (!artifact->read_from_stream(context, stream)) {
                WG_LOG_ERROR("failed to read artifact " << artifact_id);
                return StatusCode::FailedParse;
            }

            return WG_OK;
        });

        return parse_file_task.schedule(m_task_manager, read_file_async).as_async();
    }

    bool AssetArtifactCache::has(UUID artifact_id) const {
        std::lock_guard guard(m_mutex);

        auto query = m_artifacts.find(artifact_id);
        return query != m_artifacts.end();
    }

    Status AssetArtifactCache::get_info(UUID artifact_id, AssetArtifact& info) const {
        std::lock_guard guard(m_mutex);

        auto query = m_artifacts.find(artifact_id);
        if (query != m_artifacts.end()) {
            const Entry& entry = query->second;
            info.cls           = entry.cls;
            info.size          = entry.size;
            info.hash          = entry.hash;
            info.uuid          = artifact_id;
            return WG_OK;
        }

        return StatusCode::NoArtifact;
    }

    Status AssetArtifactCache::add(const Ref<RttiObject>& artifact, const std::string& name, UUID& artifact_id) {
        WG_PROFILE_CPU_ASSET("AssetArtifactCache::add");

        std::lock_guard guard(m_mutex);

        artifact_id = gen_artifact_uuid();

        IoContext context = m_io_context;

        Ref<FileMemWriter> file = make_ref<FileMemWriter>();

        std::string file_name = artifact_file_name(artifact_id);
        {
            IoStreamFile stream;
            WG_CHECKED(stream.set(file, {FileOpenMode::Out, FileOpenMode::Binary}));
            WG_CHECKED(artifact->write_to_stream(context, stream));
            WG_CHECKED(m_file_system->save_file(artifact_file_name(artifact_id), file->get_buffer()));
        }

        Entry artifact_info;
        artifact_info.name = name;
        artifact_info.cls  = artifact->get_class();
        artifact_info.size = file->get_buffer().size();
        artifact_info.hash = Sha256Builder().hash(file->get_buffer()).get();

        FileAssetArtifactMetaInfo artifact_data;
        artifact_data.name = artifact_info.name;
        artifact_data.cls  = artifact_info.cls;
        artifact_data.hash = artifact_info.hash;
        artifact_data.size = artifact_info.size;
        {
            std::string artifact_meta_data;
            IoYamlTree  artifact_meta_tree;
            WG_CHECKED(artifact_meta_tree.create_tree());
            WG_TREE_WRITE(context, artifact_meta_tree, artifact_data);
            WG_CHECKED(artifact_meta_tree.save_tree(artifact_meta_data));
            WG_CHECKED(m_file_system->save_file(artifact_meta_name(artifact_id), artifact_meta_data));
        }

        m_artifacts[artifact_id] = artifact_info;
        return WG_OK;
    }

    Status AssetArtifactCache::remove(UUID artifact_id) {
        std::lock_guard guard(m_mutex);

        auto query = m_artifacts.find(artifact_id);
        if (query == m_artifacts.end()) {
            return StatusCode::NoArtifact;
        }

        m_artifacts.erase(artifact_id);
        WG_CHECKED(m_file_system->remove_file(artifact_file_name(artifact_id)));
        WG_CHECKED(m_file_system->remove_file(artifact_meta_name(artifact_id)));

        return WG_OK;
    }

    Status AssetArtifactCache::load_cache() {
        WG_PROFILE_CPU_ASSET("AssetArtifactCache::load_cache");

        std::lock_guard guard(m_mutex);

        IoContext context = m_io_context;

        std::vector<FileEntry> files;
        WG_CHECKED(m_file_system->list_directory(m_cache_path, files));

        for (const FileEntry& file_entry : files) {
            if (file_entry.type != FileEntryType::File || !is_artifact_meta(file_entry.name)) {
                continue;
            }

            UUID artifact_id = artifact_id_from_meta(file_entry.name);

            FileAssetArtifactMetaInfo artifact_info;
            IoYamlTree                artifact_meta_tree;
            WG_CHECKED(artifact_meta_tree.parse_file(m_file_system, artifact_meta_name(artifact_id)));
            WG_TREE_READ(context, artifact_meta_tree, artifact_info);

            Entry entry;
            entry.name = artifact_info.name;
            entry.cls  = artifact_info.cls;
            entry.hash = artifact_info.hash;
            entry.size = artifact_info.size;

            m_artifacts[artifact_id] = entry;
        }

        return WG_OK;
    }

    std::string AssetArtifactCache::artifact_file_name(UUID artifact_id) {
        return m_cache_path + "/" + artifact_id.to_str() + ARTIFACT_FILE_EXT;
    }

    std::string AssetArtifactCache::artifact_meta_name(UUID artifact_id) {
        return m_cache_path + "/" + artifact_id.to_str() + ARTIFACT_FILE_META_EXT;
    }

    UUID AssetArtifactCache::gen_artifact_uuid() {
        while (true) {
            UUID id = UUID::generate();
            if (m_artifacts.find(id) == m_artifacts.end()) {
                return id;
            }
        }
    }

    UUID AssetArtifactCache::artifact_id_from_meta(const std::string& file_name) {
        return UUID(StringUtils::find_replace_first(file_name, ARTIFACT_FILE_META_EXT, ""));
    }

    bool AssetArtifactCache::is_artifact_meta(const std::string& file_name) {
        return StringUtils::is_ends_with(file_name, ARTIFACT_FILE_META_EXT);
    }

}// namespace wmoge
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
#include "io/compression.hpp"
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
        std::size_t  size_compressed;
        bool         is_compressed;
        RttiRefClass cls;
    };

    WG_RTTI_STRUCT_BEGIN(FileAssetArtifactMetaInfo) {
        WG_RTTI_FIELD(hash, {});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(size, {});
        WG_RTTI_FIELD(size_compressed, {});
        WG_RTTI_FIELD(is_compressed, {});
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

        Async read_file_async;

        if (query->second.is_compressed) {
            Ref<Data>                file_data = make_ref<Data>(query->second.size_compressed);
            array_view<std::uint8_t> file_data_buffer{file_data->buffer(), file_data->size()};

            Async fetch_file = m_async_file_system->read_file(artifact_file_name(artifact_id), file_data_buffer).as_async();

            Task decompress_artifact(SIDDBG("decompress_artifact"), [file_data, file_data_buffer, buffer](TaskContext&) -> Status {
                WG_CHECKED(Compression::decompress_lz4(file_data_buffer, buffer));
                return WG_OK;
            });

            read_file_async = decompress_artifact.schedule(m_task_manager, fetch_file).as_async();
        } else {
            read_file_async = m_async_file_system->read_file(artifact_file_name(artifact_id), buffer).as_async();
        }

        Task deserialize_artifact(SIDDBG("deserialize_artifact"), [this, artifact_id, buffer, artifact](TaskContext&) -> Status {
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

        return deserialize_artifact.schedule(m_task_manager, read_file_async).as_async();
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

        Sha256      file_hash;
        std::size_t file_size            = 0;
        std::size_t file_size_compressed = 0;
        bool        is_compressed        = false;
        std::string file_name            = artifact_file_name(artifact_id);
        {
            Ref<FileMemWriter> file = make_ref<FileMemWriter>();
            IoStreamFile       stream;
            WG_CHECKED(stream.set(file, {FileOpenMode::Out, FileOpenMode::Binary}));
            WG_CHECKED(artifact->write_to_stream(context, stream));

            std::vector<std::uint8_t> file_data = std::move(file->get_buffer());
            file_size                           = file_data.size();

            if (file_data.size() > COMPRESS_THRESHOLD) {
                is_compressed = true;

                std::size_t required_size;
                WG_CHECKED(Compression::estimate_lz4(file_data, required_size));

                std::vector<std::uint8_t> file_data_compressed(required_size);
                WG_CHECKED(Compression::compress_lz4(file_data, file_data_compressed, file_size_compressed));

                file_data_compressed.resize(file_size_compressed);
                std::swap(file_data, file_data_compressed);

                WG_LOG_INFO("compressed " << artifact_id
                                          << " from " << StringUtils::from_mem_size(file_size)
                                          << " to " << StringUtils::from_mem_size(file_size_compressed)
                                          << " ratio " << float(file_size) / (float(file_size_compressed) + 0.01f));
            }

            WG_CHECKED(m_file_system->save_file(artifact_file_name(artifact_id), file_data));
            file_hash = Sha256Builder().hash(file_data).get();
        }

        Entry artifact_info;
        artifact_info.name            = name;
        artifact_info.cls             = artifact->get_class();
        artifact_info.size            = file_size;
        artifact_info.size_compressed = file_size_compressed;
        artifact_info.is_compressed   = is_compressed;
        artifact_info.hash            = file_hash;

        FileAssetArtifactMetaInfo artifact_data;
        artifact_data.name            = artifact_info.name;
        artifact_data.cls             = artifact_info.cls;
        artifact_data.hash            = artifact_info.hash;
        artifact_data.size            = artifact_info.size;
        artifact_data.size_compressed = artifact_info.size_compressed;
        artifact_data.is_compressed   = artifact_info.is_compressed;
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
            entry.name            = artifact_info.name;
            entry.cls             = artifact_info.cls;
            entry.hash            = artifact_info.hash;
            entry.size            = artifact_info.size;
            entry.size_compressed = artifact_info.size_compressed;
            entry.is_compressed   = artifact_info.is_compressed;

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
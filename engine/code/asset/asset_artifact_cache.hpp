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

#include "asset/asset_artifact.hpp"
#include "core/array_view.hpp"
#include "core/async.hpp"
#include "core/flat_map.hpp"
#include "core/uuid.hpp"
#include "io/context.hpp"
#include "rtti/object.hpp"

#include <mutex>

namespace wmoge {
    class FileSystem;
    class IoAsyncFileSystem;
    class TaskManager;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetArtifactCache
     * @brief Asset artifacts cache for fast asset loading in dev mode
     */
    class AssetArtifactCache {
    public:
        AssetArtifactCache(std::string        cache_path,
                           IoContext          io_context,
                           FileSystem*        file_system,
                           IoAsyncFileSystem* async_file_system,
                           TaskManager*       task_manager);

        [[nodiscard]] Async read(UUID artifact_id, array_view<std::uint8_t> buffer, Ref<RttiObject> artifact);
        [[nodiscard]] bool  has(UUID artifact_id) const;
        Status              get_info(UUID artifact_id, AssetArtifact& info) const;
        Status              add(const Ref<RttiObject>& artifact, const std::string& name, UUID& artifact_id);
        Status              remove(UUID artifact_id);
        Status              load_cache();

    private:
        struct Entry {
            Sha256       hash;
            std::string  name;
            std::size_t  size            = 0;
            std::size_t  size_compressed = 0;
            bool         is_compressed   = false;
            RttiRefClass cls;
        };

        static constexpr char        ARTIFACT_FILE_EXT[]      = ".data";
        static constexpr char        ARTIFACT_FILE_META_EXT[] = ".artifact";
        static constexpr std::size_t COMPRESS_THRESHOLD       = 1024 * 4;

        [[nodiscard]] std::string artifact_file_name(UUID artifact_id);
        [[nodiscard]] std::string artifact_meta_name(UUID artifact_id);
        [[nodiscard]] UUID        gen_artifact_uuid();
        [[nodiscard]] static UUID artifact_id_from_meta(const std::string& file_name);
        [[nodiscard]] static bool is_artifact_meta(const std::string& file_name);

    private:
        flat_map<UUID, Entry> m_artifacts;
        std::string           m_cache_path;
        IoContext             m_io_context;
        FileSystem*           m_file_system       = nullptr;
        IoAsyncFileSystem*    m_async_file_system = nullptr;
        TaskManager*          m_task_manager      = nullptr;
        mutable std::mutex    m_mutex;
    };

}// namespace wmoge
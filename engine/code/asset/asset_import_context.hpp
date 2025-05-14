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

#include "asset/asset_import_env.hpp"
#include "core/array_view.hpp"
#include "core/flat_map.hpp"
#include "core/uuid.hpp"
#include "io/context.hpp"
#include "rtti/object.hpp"
#include "rtti/type_ref.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {
    class FileSystem;
    class IocContainer;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetImportContext
     * @brief Asset importing context holding results of import process
     */
    class AssetImportContext {
    public:
        using UuidProvider = std::function<UUID()>;

        AssetImportContext(std::string         path,
                           AssetImportEnv      env,
                           const UuidProvider& uuid_provider,
                           IoContext           io_context,
                           FileSystem*         file_system,
                           IocContainer*       ioc_containter);

        [[nodiscard]] UUID        alloc_asset_uuid(const std::string& asset_path);
        [[nodiscard]] std::string resolve_path(const std::string& path);
        void                      clear_deps();
        void                      add_asset_dep(AssetId asset_id);
        void                      add_asset_deps(array_view<const AssetId> asset_ids);
        void                      set_main_asset_simple(Ref<RttiObject> object, const std::string& path);
        void                      set_main_asset_simple(Ref<RttiObject> object, const std::string& path, RttiRefClass loader_cls);
        void                      set_main_asset_simple(Ref<RttiObject> object, const std::string& path, RttiRefClass asset_cls, RttiRefClass loader_cls);
        void                      set_main_asset(AssetImportAssetInfo asset);
        void                      add_child_asset(AssetImportAssetInfo asset);
        void                      add_source(std::string source_path);
        void                      add_error(AssetImportError error);

        [[nodiscard]] AssetImportResult&       get_result() { return m_result; }
        [[nodiscard]] const flat_set<AssetId>& get_asset_deps() const { return m_result.env.deps; }
        [[nodiscard]] const IoContext&         get_io_context() const { return m_io_context; }
        [[nodiscard]] FileSystem*              get_file_system() const { return m_file_system; }
        [[nodiscard]] IocContainer*            get_ioc_container() const { return m_ioc_containter; }

    private:
        AssetImportResult   m_result;
        std::string         m_path;
        flat_set<UUID>      m_ids;
        const UuidProvider& m_uuid_provider;
        IoContext           m_io_context;
        FileSystem*         m_file_system    = nullptr;
        IocContainer*       m_ioc_containter = nullptr;
    };

}// namespace wmoge
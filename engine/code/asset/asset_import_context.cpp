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

#include "asset_import_context.hpp"

#include "asset/asset_native_loader.hpp"
#include "core/string_utils.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    AssetImportContext::AssetImportContext(std::string         path,
                                           AssetImportEnv      env,
                                           const UuidProvider& uuid_provider,
                                           IoContext           io_context,
                                           FileSystem*         file_system,
                                           IocContainer*       ioc_containter)
        : m_path(std::move(path)),
          m_uuid_provider(uuid_provider),
          m_io_context(std::move(io_context)),
          m_file_system(file_system),
          m_ioc_containter(ioc_containter) {
        m_result.timestamp = DateTime::now();
        m_result.env       = std::move(env);
    }

    UUID AssetImportContext::alloc_asset_uuid(const std::string& asset_path) {
        auto       query = m_result.env.file_to_id.find(asset_path);
        const UUID id    = query != m_result.env.file_to_id.end() ? query->second : m_uuid_provider();
        if (m_ids.find(id) != m_ids.end()) {
            WG_LOG_ERROR("conflicting ids for same path " << asset_path << ", fix path uniqueness");
        }
        m_result.env.file_to_id[asset_path] = id;
        m_ids.insert(id);
        return id;
    }

    std::string AssetImportContext::resolve_path(const std::string& path) {
        if (StringUtils::is_starts_with(path, "./")) {
            return m_path + path.substr(1);
        }
        return path;
    }

    void AssetImportContext::clear_deps() {
        m_result.env.deps.clear();
    }

    void AssetImportContext::add_asset_dep(AssetId asset_id) {
        m_result.env.deps.insert(asset_id);
    }

    void AssetImportContext::add_asset_deps(array_view<const AssetId> asset_ids) {
        for (const UUID& asset_id : asset_ids) {
            add_asset_dep(asset_id);
        }
    }

    void AssetImportContext::set_main_asset_simple(Ref<RttiObject> object, const std::string& path) {
        RttiRefClass asset_cls  = object->get_class();
        RttiRefClass loader_cls = AssetNativeLoader::get_class_static();
        set_main_asset_simple(std::move(object), path, asset_cls, loader_cls);
    }

    void AssetImportContext::set_main_asset_simple(Ref<RttiObject> object, const std::string& path, RttiRefClass loader_cls) {
        RttiRefClass asset_cls = object->get_class();
        set_main_asset_simple(std::move(object), path, asset_cls, loader_cls);
    }

    void AssetImportContext::set_main_asset_simple(Ref<RttiObject> object, const std::string& path, RttiRefClass asset_cls, RttiRefClass loader_cls) {
        AssetImportArtifactInfo artifact;
        artifact.object = std::move(object);
        artifact.name   = path;

        AssetImportAssetInfo asset;
        asset.path   = path;
        asset.uuid   = alloc_asset_uuid(path);
        asset.cls    = asset_cls;
        asset.loader = loader_cls;
        asset.artifacts.push_back(std::move(artifact));

        set_main_asset(std::move(asset));
        add_source(path);
    }

    void AssetImportContext::set_main_asset(AssetImportAssetInfo asset) {
        m_result.main = std::move(asset);
    }

    void AssetImportContext::add_child_asset(AssetImportAssetInfo asset) {
        m_result.children.push_back(std::move(asset));
    }

    void AssetImportContext::add_source(std::string source_path) {
        m_result.sources.push_back(std::move(source_path));
    }

    void AssetImportContext::add_error(AssetImportError error) {
        m_result.env.errors.push_back(std::move(error));
    }

}// namespace wmoge
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

#include "shader_asset_loader.hpp"

#include "grc/shader.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_manager.hpp"
#include "io/tree_yaml.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    Status ShaderAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Shader>& asset) {
        WG_AUTO_PROFILE_ASSET("ShaderAssetLoader::load_typed");

        Ref<AssetImportData> import_data = context.asset_meta.import_data.cast<AssetImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data to load " << asset_id);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << asset_id);
            return StatusCode::InvalidData;
        }

        std::string path_on_disk = import_data->source_files[0].file;
        if (path_on_disk.empty()) {
            WG_LOG_ERROR("no path on disk to load asset file " << asset_id);
            return StatusCode::InvalidData;
        }

        ShaderFile shader_file;
        IoYamlTree tree;
        WG_CHECKED(tree.parse_file(path_on_disk));
        WG_TREE_READ(context.io_context, tree, shader_file);

        auto* shader_manager = IocContainer::iresolve_v<ShaderManager>();

        ShaderReflection shader_reflection;
        WG_CHECKED(shader_manager->load_shader_reflection(shader_file, shader_reflection));

        asset = make_ref<Shader>(std::move(shader_reflection));
        asset->set_id(asset_id);
        shader_manager->add_shader(asset);

        return WG_OK;
    }

}// namespace wmoge
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

#include "shader_importer.hpp"

#include "core/ioc_container.hpp"
#include "grc/shader.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_loader.hpp"
#include "grc/shader_manager.hpp"
#include "grc/shader_reflection.hpp"
#include "io/tree_yaml.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status ShaderImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"shader"};
        return WG_OK;
    }

    Status ShaderImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        auto& preset    = presets.emplace_back();
        preset.name     = "default";
        preset.settings = make_ref<AssetImportSettings>();
        return WG_OK;
    }

    Status ShaderImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status ShaderImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("ShaderImporter::import");

        ShaderFile shader_file;
        {
            IoYamlTree tree;
            if (!tree.parse_file(context.get_file_system(), path)) {
                WG_LOG_ERROR("failed parse file " << path);
                return StatusCode::FailedParse;
            }

            IoContext io_context = context.get_io_context();

            if (!tree_read(io_context, tree, shader_file)) {
                WG_LOG_ERROR("failed decode file " << path);
                return StatusCode::FailedParse;
            }
        }

        ShaderManager* shader_manager = context.get_ioc_container()->resolve_value<ShaderManager>();

        ShaderReflection reflection;
        if (!shader_manager->load_shader_reflection(shader_file, reflection)) {
            WG_LOG_ERROR("failed to load reflection from file " << path);
            return StatusCode::InvalidData;
        }

        Ref<Shader> shader = make_ref<Shader>(std::move(reflection));

        context.set_main_asset_simple(shader, path, ShaderLoader::get_class_static());
        return WG_OK;
    }

}// namespace wmoge
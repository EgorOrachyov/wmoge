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

#include "ui_markup_importer.hpp"

#include "asset/asset_native_loader.hpp"
#include "core/ioc_container.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"
#include "rtti/type_storage.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_markup.hpp"
#include "ui/ui_markup_xml_parser.hpp"

namespace wmoge {

    Status UiMarkupImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"uixml"};
        return WG_OK;
    }

    Status UiMarkupImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        auto& preset    = presets.emplace_back();
        preset.name     = "default";
        preset.settings = make_ref<AssetImportSettings>();
        return WG_OK;
    }

    Status UiMarkupImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status UiMarkupImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("UiMarkupImporter::import");

        FileSystem* file_system = context.get_file_system();

        std::vector<std::uint8_t> file_content;
        if (!file_system->read_file(path, file_content)) {
            WG_LOG_ERROR("failed to read file with markup " << path);
            return StatusCode::FailedRead;
        }

        RttiTypeStorage* type_storage = context.get_ioc_container()->resolve_value<RttiTypeStorage>();

        UiMarkupDecs desc;

        UiMarkupParser parser(path, desc, file_content, type_storage);
        if (!parser.parse()) {
            WG_LOG_ERROR("failed to parse markup from " << path);
            return StatusCode::FailedParse;
        }

        Ref<UiMarkup> markup = make_ref<UiMarkup>();
        markup->set_desc(std::move(desc));

        context.set_main_asset_simple(markup, path);
        return WG_OK;
    }

}// namespace wmoge
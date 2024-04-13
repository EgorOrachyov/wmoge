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

#include "freetype_asset_loader.hpp"

#include "asset/font.hpp"
#include "debug/profiler.hpp"
#include "freetype_font.hpp"

namespace wmoge {

    Status FreetypeAssetLoader::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& res) {
        WG_AUTO_PROFILE_ASSET("FreetypeAssetLoader::load");

        Ref<Font> font = meta.cls->instantiate().cast<Font>();

        if (!font) {
            WG_LOG_ERROR("failed to instantiate font " << name);
            return StatusCode::FailedInstantiate;
        }

        res = font;
        res->set_name(name);

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("no import options to load font " << name);
            return StatusCode::InvalidData;
        }

        FontImportOptions options;
        WG_YAML_READ_AS(meta.import_options->crootref(), "params", options);

        return FreetypeFont::load(font, options.source_file, options.height, options.glyphs_in_row);
    }
    Strid FreetypeAssetLoader::get_name() {
        return SID("freetype");
    }

}// namespace wmoge
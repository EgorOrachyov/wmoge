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

#include "asset/asset_flags.hpp"
#include "asset/asset_import_env.hpp"
#include "asset/asset_import_settings.hpp"
#include "core/uuid.hpp"
#include "rtti/traits.hpp"
#include "rtti/type_ref.hpp"

namespace wmoge {

    /**
     * @class AssetMetaData
     * @brief Asset meta data stored along with asset source file(s)
     */
    struct AssetMetaData {
        WG_RTTI_STRUCT(AssetMetaData)

        UUID                     uuid;
        AssetFlags               flags;
        RttiRefClass             importer;
        AssetImportEnv           import_env;
        Ref<AssetImportSettings> import_settings;
    };

    WG_RTTI_STRUCT_BEGIN(AssetMetaData) {
        WG_RTTI_EXTENSION(".asset");
        WG_RTTI_FIELD(uuid, {RttiOptional});
        WG_RTTI_FIELD(flags, {RttiOptional});
        WG_RTTI_FIELD(importer, {});
        WG_RTTI_FIELD(import_env, {RttiOptional});
        WG_RTTI_FIELD(import_settings, {});
    }
    WG_RTTI_END;

}// namespace wmoge
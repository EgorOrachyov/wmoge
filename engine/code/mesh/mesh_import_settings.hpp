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

#include "asset/asset_import_settings.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_defs.hpp"

namespace wmoge {

    /**
     * @class MeshImportProcess
     * @brief Controls post-process of imported data
     */
    struct MeshImportProcess {
        WG_RTTI_STRUCT(MeshImportProcess);

        bool triangulate             = true;
        bool tangent_space           = false;
        bool flip_uv                 = true;
        bool gen_normals             = false;
        bool gen_smooth_normals      = false;
        bool join_identical_vertices = true;
        bool limit_bone_weights      = true;
        bool improve_cache_locality  = false;
        bool sort_by_ptype           = true;
        bool gen_uv                  = false;
    };

    WG_RTTI_STRUCT_BEGIN(MeshImportProcess) {
        WG_RTTI_FIELD(triangulate, {RttiOptional});
        WG_RTTI_FIELD(tangent_space, {RttiOptional});
        WG_RTTI_FIELD(flip_uv, {RttiOptional});
        WG_RTTI_FIELD(gen_normals, {RttiOptional});
        WG_RTTI_FIELD(gen_smooth_normals, {RttiOptional});
        WG_RTTI_FIELD(join_identical_vertices, {RttiOptional});
        WG_RTTI_FIELD(limit_bone_weights, {RttiOptional});
        WG_RTTI_FIELD(improve_cache_locality, {RttiOptional});
        WG_RTTI_FIELD(sort_by_ptype, {RttiOptional});
        WG_RTTI_FIELD(gen_uv, {RttiOptional});
    }
    WG_RTTI_END;

    /** 
     * @class MeshImportSettings
     * @brief Settings to import a mesh
     */
    class MeshImportSettings : public AssetImportSettings {
    public:
        WG_RTTI_CLASS(MeshImportSettings, AssetImportSettings)

        Mask<GfxVertAttrib> attributes = {GfxVertAttrib::Pos3f};
        MeshImportProcess   process{};
    };

    WG_RTTI_CLASS_BEGIN(MeshImportSettings) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(attributes, {RttiOptional});
        WG_RTTI_FIELD(process, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge
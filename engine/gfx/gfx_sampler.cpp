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

#include "gfx_sampler.hpp"

#include "core/crc32.hpp"
#include "io/yaml.hpp"

#include <cstring>
#include <sstream>

namespace wmoge {

    GfxSamplerDesc::GfxSamplerDesc() {
        std::memset(this, 0, sizeof(GfxSamplerDesc));
        min_lod        = 0;
        max_lod        = 32.0f;
        max_anisotropy = 0.0f;
        min_flt        = GfxSampFlt::Nearest;
        mag_flt        = GfxSampFlt::Nearest;
        u              = GfxSampAddress::Repeat;
        v              = GfxSampAddress::Repeat;
        w              = GfxSampAddress::Repeat;
        brd_clr        = GfxSampBrdClr::Black;
    }
    bool GfxSamplerDesc::operator==(const GfxSamplerDesc& other) const {
        return std::memcmp(this, &other, sizeof(GfxSamplerDesc)) == 0;
    }
    std::size_t GfxSamplerDesc::hash() const {
        return static_cast<std::size_t>(Crc32::hash(this, sizeof(GfxSamplerDesc)));
    }
    std::string GfxSamplerDesc::to_str() const {
        std::stringstream sampler_name;
        sampler_name << magic_enum::enum_name(min_flt) << ":" << magic_enum::enum_name(mag_flt) << ","
                     << magic_enum::enum_name(u) << ":" << magic_enum::enum_name(v) << ":" << magic_enum::enum_name(w) << ","
                     << min_lod << ":" << max_lod << ","
                     << max_anisotropy;

        return sampler_name.str();
    }

    bool yaml_read(const YamlConstNodeRef& node, GfxSamplerDesc& desc) {
        WG_YAML_READ_AS_OPT(node, "min_lod", desc.min_lod);
        WG_YAML_READ_AS_OPT(node, "max_lod", desc.max_lod);
        WG_YAML_READ_AS_OPT(node, "max_anisotropy", desc.max_anisotropy);
        WG_YAML_READ_AS_OPT(node, "min_flt", desc.min_flt);
        WG_YAML_READ_AS_OPT(node, "mag_flt", desc.mag_flt);
        WG_YAML_READ_AS_OPT(node, "u", desc.u);
        WG_YAML_READ_AS_OPT(node, "v", desc.v);
        WG_YAML_READ_AS_OPT(node, "w", desc.w);
        WG_YAML_READ_AS_OPT(node, "brd_clr", desc.brd_clr);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const GfxSamplerDesc& desc) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "min_lod", desc.min_lod);
        WG_YAML_WRITE_AS(node, "max_lod", desc.max_lod);
        WG_YAML_WRITE_AS(node, "max_anisotropy", desc.max_anisotropy);
        WG_YAML_WRITE_AS(node, "min_flt", desc.min_flt);
        WG_YAML_WRITE_AS(node, "mag_flt", desc.mag_flt);
        WG_YAML_WRITE_AS(node, "u", desc.u);
        WG_YAML_WRITE_AS(node, "v", desc.v);
        WG_YAML_WRITE_AS(node, "w", desc.w);
        WG_YAML_WRITE_AS(node, "brd_clr", desc.brd_clr);

        return true;
    }

}// namespace wmoge
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

    WG_IO_BEGIN(GfxSamplerDesc)
    WG_IO_FIELD(min_lod)
    WG_IO_FIELD(max_lod)
    WG_IO_FIELD(max_anisotropy)
    WG_IO_FIELD(min_flt)
    WG_IO_FIELD(mag_flt)
    WG_IO_FIELD(u)
    WG_IO_FIELD(v)
    WG_IO_FIELD(w)
    WG_IO_FIELD(brd_clr)
    WG_IO_END(GfxSamplerDesc)

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
    GfxSamplerDesc GfxSamplerDesc::make(GfxSampFlt flt, float aniso, GfxSampAddress address) {
        GfxSamplerDesc d;
        d.min_flt        = flt;
        d.mag_flt        = flt;
        d.max_anisotropy = aniso;
        d.u              = address;
        d.v              = address;
        d.w              = address;
        return d;
    }
    GfxSamplerDesc GfxSamplerDesc::make(GfxSampFlt flt, float aniso, GfxSampBrdClr brd_clr) {
        GfxSamplerDesc d;
        d.min_flt        = flt;
        d.mag_flt        = flt;
        d.max_anisotropy = aniso;
        d.u              = GfxSampAddress::ClampToBorder;
        d.v              = GfxSampAddress::ClampToBorder;
        d.w              = GfxSampAddress::ClampToBorder;
        d.brd_clr        = brd_clr;
        return d;
    }

}// namespace wmoge
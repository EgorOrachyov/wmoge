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

#ifndef WMOGE_GFX_SAMPLER_HPP
#define WMOGE_GFX_SAMPLER_HPP

#include "gfx/gfx_resource.hpp"
#include "io/yaml.hpp"

#include <utility>

namespace wmoge {

    /**
     * @class GfxSamplerDesc
     * @brief Gfx sampler descriptor
     */
    struct GfxSamplerDesc {
        GfxSamplerDesc();
        bool        operator==(const GfxSamplerDesc& other) const;
        std::size_t hash() const;
        std::string to_str() const;

        float          min_lod;       // = 0;
        float          max_lod;       // = 32.0f;
        float          max_anisotropy;// = 0.0f;
        GfxSampFlt     min_flt;       // = GfxSampFlt::Nearest;
        GfxSampFlt     mag_flt;       // = GfxSampFlt::Nearest;
        GfxSampAddress u;             // = GfxSampAddress::Repeat;
        GfxSampAddress v;             // = GfxSampAddress::Repeat;
        GfxSampAddress w;             // = GfxSampAddress::Repeat;
        GfxSampBrdClr  brd_clr;       // = GfxSampBrdClr::Black;

        friend bool yaml_read(const YamlConstNodeRef& node, GfxSamplerDesc& desc);
        friend bool yaml_write(YamlNodeRef node, const GfxSamplerDesc& desc);
    };

    /**
     * @class GfxSampler
     * @brief Gfx sampler for sampling textures in shaders
     */
    class GfxSampler : public GfxResource {
    public:
        ~GfxSampler() override                     = default;
        virtual const GfxSamplerDesc& desc() const = 0;
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::GfxSamplerDesc> {
        std::size_t operator()(const wmoge::GfxSamplerDesc& desc) const {
            return desc.hash();
        }
    };

}// namespace std

#endif//WMOGE_GFX_SAMPLER_HPP

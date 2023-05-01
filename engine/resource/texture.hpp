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

#ifndef WMOGE_TEXTURE_HPP
#define WMOGE_TEXTURE_HPP

#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "resource/image.hpp"
#include "resource/resource.hpp"

namespace wmoge {

    /**
     * @class Texture
     * @brief Base-class for any engine gpu texture resource which can be used for rendering
     */
    class Texture : public Resource {
    public:
        WG_OBJECT(Texture, Resource);

        const std::vector<ref_ptr<Image>>& get_images() { return m_images; }
        const ref_ptr<GfxTexture>&         get_texture() { return m_texture; }
        const ref_ptr<GfxSampler>&         get_sampler() { return m_sampler; }
        int                                get_width() { return m_width; }
        int                                get_height() { return m_height; }
        int                                get_depth() { return m_depth; }
        int                                get_array_slices() { return m_array_slices; }
        int                                get_mips() { return m_mips; }
        GfxTex                             get_tex_type() { return m_tex_type; }
        GfxFormat                          get_format() { return m_format; }
        GfxMemUsage                        get_mem_usage() { return m_mem_usage; }
        GfxTexUsages                       get_usages() { return m_usages; }
        bool                               get_srgb() { return m_srgb; }
        bool                               get_compression() { return m_compression; }

        void copy_to(Resource& copy) override;

    protected:
        void load_sampler_from_import_options(const YamlTree& tree);

    protected:
        std::vector<ref_ptr<Image>> m_images;
        ref_ptr<GfxTexture>         m_texture;
        ref_ptr<GfxSampler>         m_sampler;
        int                         m_width        = 0;
        int                         m_height       = 0;
        int                         m_depth        = 0;
        int                         m_array_slices = 0;
        int                         m_mips         = 0;
        GfxTex                      m_tex_type;
        GfxFormat                   m_format;
        GfxMemUsage                 m_mem_usage;
        GfxTexUsages                m_usages;
        bool                        m_srgb        = false;
        bool                        m_compression = false;
    };

    /**
     * @class Texture2d
     * @brief 2d gpu texture resource
     */
    class Texture2d final : public Texture {
    public:
        WG_OBJECT(Texture2d, Texture);

        void create(const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler);

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;
    };

    /**
     * @class TextureCube
     * @brief Cube-map gpu texture resource
     */
    class TextureCube final : public Texture {
    public:
        WG_OBJECT(TextureCube, Texture);

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;
    };

}// namespace wmoge

#endif//WMOGE_TEXTURE_HPP

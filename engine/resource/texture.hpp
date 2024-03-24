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

#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/grc_texture_compression.hpp"
#include "grc/grc_texture_resize.hpp"
#include "io/serialization.hpp"
#include "resource/image.hpp"
#include "resource/resource.hpp"

namespace wmoge {

    /**
     * @class TextureImportOptions
     * @brief Options (base) to import texture
     */
    struct TextureImportOptions {
        int                     channels = 4;
        GfxFormat               format   = GfxFormat::RGBA8;
        bool                    mipmaps  = true;
        bool                    srgb     = true;
        GfxSamplerDesc          sampling{};
        GrcTexCompressionParams compression{};
        GrcTexResizeParams      resizing{};

        WG_IO_DECLARE(TextureImportOptions);
    };

    /**
     * @class Texture2dImportOptions
     * @brief Options to import 2d-texture from a source file
     */
    struct Texture2dImportOptions : public TextureImportOptions {
        std::string source_file;

        WG_IO_DECLARE(Texture2dImportOptions);
    };

    /**
     * @class TextureCubeImportOptions
     * @brief Options to import a cube-map texture from source files
     */
    struct TextureCubeImportOptions : public TextureImportOptions {
        /**
         * @class SourceFiles
         * @brief Files fro each cube-map face
         */
        struct SourceFiles {
            std::string right;
            std::string left;
            std::string top;
            std::string bottom;
            std::string back;
            std::string front;

            WG_IO_DECLARE(SourceFiles);
        };

        SourceFiles source_files;

        WG_IO_DECLARE(TextureCubeImportOptions);
    };

    /**
     * @class Texture
     * @brief Base-class for any engine gpu texture resource which can be used for rendering
     */
    class Texture : public Resource {
    public:
        WG_OBJECT(Texture, Resource);

        /**
         * @brief Create new texture of desired format and size
         *
         * @param format Base (with no compression) texture format
         * @param width Width of the texture in pixels
         * @param height Height of the texture in pixels
         * @param depth Depth of the texture in pixels (in most cases 1)
         * @param array_slices Number of slices for array or cube texture (in most cases 1)
         * @param swizz Texture channels swizzling
         */
        Texture(GfxFormat format, int width, int height, int depth = 1, int array_slices = 1, GfxTexSwizz swizz = GfxTexSwizz::None);

        virtual void set_source_images(std::vector<Ref<Image>> images);
        virtual void set_sampler(const Ref<GfxSampler>& sampler);
        virtual void set_sampler_from_desc(const GfxSamplerDesc& desc);
        virtual void set_compression(const GrcTexCompressionParams& params);

        /** @brief Generate mip-chain for the image using source 0-mip faces data */
        virtual Status generate_mips();
        /** @brief Generate compressed texture data based on compression settings */
        virtual Status generate_compressed_data();
        /** @brief Create default gfx texture resource and sampler */
        virtual Status generate_gfx_resource();

        [[nodiscard]] const std::vector<Ref<Image>>&   get_images() const { return m_images; }
        [[nodiscard]] const std::vector<GfxImageData>& get_compressed() const { return m_compressed; }
        [[nodiscard]] const Ref<GfxTexture>&           get_texture() const { return m_texture; }
        [[nodiscard]] const Ref<GfxSampler>&           get_sampler() const { return m_sampler; }
        [[nodiscard]] int                              get_width() const { return m_width; }
        [[nodiscard]] int                              get_height() const { return m_height; }
        [[nodiscard]] int                              get_depth() const { return m_depth; }
        [[nodiscard]] int                              get_array_slices() const { return m_array_slices; }
        [[nodiscard]] int                              get_mips() const { return m_mips; }
        [[nodiscard]] GfxFormat                        get_format() const { return m_format; }
        [[nodiscard]] GfxFormat                        get_format_compressed() const { return m_format_compressed; }
        [[nodiscard]] GfxTex                           get_tex_type() const { return m_tex_type; }
        [[nodiscard]] GfxTexSwizz                      get_tex_swizz() const { return m_swizz; }
        [[nodiscard]] GfxMemUsage                      get_mem_usage() const { return m_mem_usage; }
        [[nodiscard]] GfxTexUsages                     get_usages() const { return m_usages; }
        [[nodiscard]] bool                             get_srgb() const { return m_srgb; }
        [[nodiscard]] const GrcTexCompressionParams&   get_compression() const { return m_compression; }

        Status copy_to(Object& other) const override;

    protected:
        std::vector<Ref<Image>>   m_images;
        std::vector<GfxImageData> m_compressed;
        Ref<GfxTexture>           m_texture;
        Ref<GfxSampler>           m_sampler;
        int                       m_width             = 0;
        int                       m_height            = 0;
        int                       m_depth             = 0;
        int                       m_array_slices      = 0;
        int                       m_mips              = 0;
        GfxFormat                 m_format            = GfxFormat::Unknown;
        GfxFormat                 m_format_compressed = GfxFormat::Unknown;
        GfxTex                    m_tex_type          = GfxTex::Tex2d;
        GfxTexSwizz               m_swizz             = GfxTexSwizz::None;
        GfxMemUsage               m_mem_usage         = GfxMemUsage::GpuLocal;
        GfxTexUsages              m_usages            = {GfxTexUsageFlag::Sampling};
        bool                      m_srgb              = false;
        GrcTexCompressionParams   m_compression{};
    };

    /**
     * @class Texture2d
     * @brief 2d gpu texture resource
     */
    class Texture2d final : public Texture {
    public:
        WG_OBJECT(Texture2d, Texture);

        Texture2d(GfxFormat format, int width, int height, GfxTexSwizz swizz = GfxTexSwizz::None);

        Status copy_to(Object& other) const override;
    };

    /**
     * @class TextureCube
     * @brief Cube-map gpu texture resource
     */
    class TextureCube final : public Texture {
    public:
        WG_OBJECT(TextureCube, Texture);

        TextureCube(GfxFormat format, int width, int height);

        Status copy_to(Object& other) const override;
    };

}// namespace wmoge
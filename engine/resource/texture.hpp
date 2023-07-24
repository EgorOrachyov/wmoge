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
     * @class TextureImportOptions
     * @brief Options (base) to import texture
     */
    struct TextureImportOptions {
        int            channels    = 4;
        GfxFormat      format      = GfxFormat::RGBA8;
        bool           mipmaps     = true;
        bool           srgb        = true;
        bool           compression = false;
        GfxSamplerDesc sampling{};

        friend bool yaml_read(const YamlConstNodeRef& node, TextureImportOptions& options);
        friend bool yaml_write(YamlNodeRef node, const TextureImportOptions& options);
    };

    /**
     * @class Texture2dImportOptions
     * @brief Options to import 2d-texture from a source file
     */
    struct Texture2dImportOptions : public TextureImportOptions {
        std::string source_file;

        friend bool yaml_read(const YamlConstNodeRef& node, Texture2dImportOptions& options);
        friend bool yaml_write(YamlNodeRef node, const Texture2dImportOptions& options);
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

            friend bool yaml_read(const YamlConstNodeRef& node, SourceFiles& source_files);
            friend bool yaml_write(YamlNodeRef node, const SourceFiles& source_files);
        };

        SourceFiles source_files;

        friend bool yaml_read(const YamlConstNodeRef& node, TextureCubeImportOptions& options);
        friend bool yaml_write(YamlNodeRef node, const TextureCubeImportOptions& options);
    };

    /**
     * @class Texture
     * @brief Base-class for any engine gpu texture resource which can be used for rendering
     */
    class Texture : public Resource {
    public:
        WG_OBJECT(Texture, Resource);

        const std::vector<Ref<Image>>& get_images() { return m_images; }
        const Ref<GfxTexture>&         get_texture() { return m_texture; }
        const Ref<GfxSampler>&         get_sampler() { return m_sampler; }
        int                            get_width() { return m_width; }
        int                            get_height() { return m_height; }
        int                            get_depth() { return m_depth; }
        int                            get_array_slices() { return m_array_slices; }
        int                            get_mips() { return m_mips; }
        GfxTex                         get_tex_type() { return m_tex_type; }
        GfxFormat                      get_format() { return m_format; }
        GfxMemUsage                    get_mem_usage() { return m_mem_usage; }
        GfxTexUsages                   get_usages() { return m_usages; }
        bool                           get_srgb() { return m_srgb; }
        bool                           get_compression() { return m_compression; }

        void copy_to(Resource& copy) override;

    protected:
        std::vector<Ref<Image>> m_images;
        Ref<GfxTexture>         m_texture;
        Ref<GfxSampler>         m_sampler;
        int                     m_width        = 0;
        int                     m_height       = 0;
        int                     m_depth        = 0;
        int                     m_array_slices = 0;
        int                     m_mips         = 0;
        GfxTex                  m_tex_type;
        GfxFormat               m_format;
        GfxMemUsage             m_mem_usage;
        GfxTexUsages            m_usages;
        bool                    m_srgb        = false;
        bool                    m_compression = false;
    };

    /**
     * @class Texture2d
     * @brief 2d gpu texture resource
     */
    class Texture2d final : public Texture {
    public:
        WG_OBJECT(Texture2d, Texture);

        void create(const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler);

        void copy_to(Resource& copy) override;
    };

    /**
     * @class TextureCube
     * @brief Cube-map gpu texture resource
     */
    class TextureCube final : public Texture {
    public:
        WG_OBJECT(TextureCube, Texture);

        void create(const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler);

        void copy_to(Resource& copy) override;
    };

}// namespace wmoge

#endif//WMOGE_TEXTURE_HPP

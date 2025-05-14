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

#include "asset/asset.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/image.hpp"
#include "grc/texture_compression.hpp"
#include "grc/texture_pool.hpp"
#include "grc/texture_resize.hpp"
#include "io/serialization.hpp"

namespace wmoge {

    /** @brief Flag assigned to texture asset */
    enum class TextureFlag {
        Managed = 0,// Texture managed withing texture manager
        Pooled,     // Texture gpu memory allocated from pool
        Streamed,   // Texture can be streamed in-out from disk
        Compressed, // Texture uses gpu compression to reduce footprint
        FromDisk,   // Texture loaded from disc as an asset
        Font        // Texture created as a font glyph atlas
    };

    /** @brief Built-in default textures */
    enum class DefaultTexture {
        White = 0,
        Black,
        Red,
        Green,
        Blue,
        Gray,
        Total
    };

    /** @brief Built-in default samplers */
    enum class DefaultSampler {
        Default = 0,
        Linear,
        Nearest,
        Total
    };

    /** @brief Flags assigned to texture asset */
    using TextureFlags = Mask<TextureFlag>;

    /** @brief Texture asset base params for construction */
    struct TextureParams {
        WG_RTTI_STRUCT(TextureParams)

        int                  width         = 0;
        int                  height        = 0;
        int                  depth         = 1;
        int                  array_slices  = 1;
        int                  mips          = 1;
        GfxFormat            format        = GfxFormat::Unknown;
        GfxFormat            format_source = GfxFormat::Unknown;
        GfxTex               tex_type      = GfxTex::Tex2d;
        GfxTexSwizz          swizz         = GfxTexSwizz::None;
        GfxMemUsage          mem_usage     = GfxMemUsage::GpuLocal;
        GfxTexUsages         usages        = {GfxTexUsageFlag::Sampling};
        bool                 srgb          = false;
        TexCompressionParams compression{};
        TextureFlags         flags;
    };

    WG_RTTI_STRUCT_BEGIN(TextureParams) {
        WG_RTTI_FIELD(width, {});
        WG_RTTI_FIELD(height, {});
        WG_RTTI_FIELD(depth, {});
        WG_RTTI_FIELD(array_slices, {});
        WG_RTTI_FIELD(mips, {});
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(format_source, {});
        WG_RTTI_FIELD(tex_type, {});
        WG_RTTI_FIELD(swizz, {});
        WG_RTTI_FIELD(mem_usage, {});
        WG_RTTI_FIELD(usages, {});
        WG_RTTI_FIELD(srgb, {});
        WG_RTTI_FIELD(compression, {});
        WG_RTTI_FIELD(flags, {});
    }
    WG_RTTI_END;

    /** 
     * @class TextureDesc
     * @brief Texture asset desc for construction
     */
    struct TextureDesc {
        WG_RTTI_STRUCT(TextureDesc)

        std::vector<Ref<Image>>   images;
        std::vector<GfxImageData> compressed;
        std::string               name;
        DefaultSampler            sampler;
        TextureParams             params;
    };

    WG_RTTI_STRUCT_BEGIN(TextureDesc) {
        WG_RTTI_FIELD(images, {});
        WG_RTTI_FIELD(compressed, {});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(sampler, {});
        WG_RTTI_FIELD(params, {});
    }
    WG_RTTI_END;

    /**
     * @class Texture
     * @brief Base-class for any engine gpu texture asset which can be used for rendering
     */
    class Texture : public Asset {
    public:
        WG_RTTI_CLASS(Texture, Asset)

        using Callback    = std::function<void(Texture*)>;
        using CallbackRef = std::shared_ptr<Callback>;

        Texture() = default;
        ~Texture() override;

        /**
         * @brief Creates texture object from desc
         * 
         * @brief desc Texture desc
         */
        Texture(TextureDesc&& desc);

        void set_source_images(std::vector<Ref<Image>> images, GfxFormat format);
        void set_texture(const Ref<GfxTexture>& texture);
        void set_sampler(const Ref<GfxSampler>& sampler);
        void set_texture_callback(CallbackRef callback);

        [[nodiscard]] const std::vector<Ref<Image>>&   get_images() const { return m_desc.images; }
        [[nodiscard]] const std::vector<GfxImageData>& get_compressed() const { return m_desc.compressed; }
        [[nodiscard]] const std::string&               get_name() const { return m_desc.name; }
        [[nodiscard]] const Ref<GfxTexture>&           get_texture() const { return m_texture; }
        [[nodiscard]] const Ref<GfxSampler>&           get_sampler() const { return m_sampler; }
        [[nodiscard]] int                              get_width() const { return m_desc.params.width; }
        [[nodiscard]] int                              get_height() const { return m_desc.params.height; }
        [[nodiscard]] int                              get_depth() const { return m_desc.params.depth; }
        [[nodiscard]] int                              get_array_slices() const { return m_desc.params.array_slices; }
        [[nodiscard]] int                              get_mips() const { return m_desc.params.mips; }
        [[nodiscard]] GfxFormat                        get_format() const { return m_desc.params.format; }
        [[nodiscard]] GfxFormat                        get_format_source() const { return m_desc.params.format_source; }
        [[nodiscard]] GfxTex                           get_tex_type() const { return m_desc.params.tex_type; }
        [[nodiscard]] GfxTexSwizz                      get_tex_swizz() const { return m_desc.params.swizz; }
        [[nodiscard]] GfxMemUsage                      get_mem_usage() const { return m_desc.params.mem_usage; }
        [[nodiscard]] GfxTexUsages                     get_usages() const { return m_desc.params.usages; }
        [[nodiscard]] bool                             get_srgb() const { return m_desc.params.srgb; }
        [[nodiscard]] const TexCompressionParams&      get_compression() const { return m_desc.params.compression; }
        [[nodiscard]] const TextureFlags&              get_flags() const { return m_desc.params.flags; }
        [[nodiscard]] GfxTextureDesc                   get_gfx_desc() const;
        [[nodiscard]] const TextureDesc&               get_desc() const { return m_desc; }

    protected:
        TextureDesc     m_desc;
        Ref<GfxTexture> m_texture;
        Ref<GfxSampler> m_sampler;
        CallbackRef     m_callback;
    };

    WG_RTTI_CLASS_BEGIN(Texture) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_desc, {});
    }
    WG_RTTI_END;

    /**
     * @class Texture2d
     * @brief 2d gpu texture asset
     */
    class Texture2d final : public Texture {
    public:
        WG_RTTI_CLASS(Texture2d, Texture);

        Texture2d() = default;
        Texture2d(TextureDesc&& desc);
    };

    WG_RTTI_CLASS_BEGIN(Texture2d) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class TextureCube
     * @brief Cube-map gpu texture asset
     */
    class TextureCube final : public Texture {
    public:
        WG_RTTI_CLASS(TextureCube, Texture);

        TextureCube() = default;
        TextureCube(TextureDesc&& desc);
    };

    WG_RTTI_CLASS_BEGIN(TextureCube) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class TextureCubeFile
     * @brief Struct for loading cube textures from .texcube file
     */
    struct TextureCubeFile {
        WG_RTTI_STRUCT(TextureCubeFile)

        std::string right;
        std::string left;
        std::string top;
        std::string bottom;
        std::string front;
        std::string back;
    };

    WG_RTTI_STRUCT_BEGIN(TextureCubeFile) {
        WG_RTTI_FIELD(right, {});
        WG_RTTI_FIELD(left, {});
        WG_RTTI_FIELD(top, {});
        WG_RTTI_FIELD(bottom, {});
        WG_RTTI_FIELD(front, {});
        WG_RTTI_FIELD(back, {});
    }
    WG_RTTI_END;

    void rtti_grc_texture();

}// namespace wmoge
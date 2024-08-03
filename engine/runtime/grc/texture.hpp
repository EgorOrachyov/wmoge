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
        Font,       // Texture created as a font glyph atlas
    };

    /** @brief Flags assigned to texture asset */
    using TextureFlags = Mask<TextureFlag>;

    /**
     * @class Texture
     * @brief Base-class for any engine gpu texture asset which can be used for rendering
     */
    class Texture : public Asset {
    public:
        WG_RTTI_CLASS(Texture, Asset);

        using Callback    = std::function<void(Texture*)>;
        using CallbackRef = std::shared_ptr<Callback>;

        Texture() = default;
        ~Texture() override;

        /**
         * @brief Creates texture object from flags and gfx desc
         * 
         * @brief flags Texture object usage flags
         * @brief desc Gfx texture desc
         */
        Texture(TextureFlags flags, GfxTextureDesc desc);

        void set_source_images(std::vector<Ref<Image>> images);
        void set_sampler(const Ref<GfxSampler>& sampler);
        void set_sampler_from_desc(const GfxSamplerDesc& desc);
        void set_compression(const TexCompressionParams& params);
        void set_flags(const TextureFlags& flags);
        void set_texture_callback(CallbackRef callback);

        Status generate_mips();
        Status generate_compressed_data();
        Status create_gfx_resource(TexturePool& pool);
        Status delete_gfx_resource(TexturePool& pool);
        Status upload_gfx_data(GfxCmdListRef& cmd);

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
        [[nodiscard]] const TexCompressionParams&      get_compression() const { return m_compression; }
        [[nodiscard]] const TextureFlags&              get_flgas() const { return m_flags; }
        [[nodiscard]] GfxTextureDesc                   get_desc() const;

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
        TexCompressionParams      m_compression{};
        TextureFlags              m_flags;
        CallbackRef               m_callback;
    };

    WG_RTTI_CLASS_BEGIN(Texture) {
        WG_RTTI_META_DATA(RttiUiHint(""));
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class Texture2d
     * @brief 2d gpu texture asset
     */
    class Texture2d final : public Texture {
    public:
        WG_RTTI_CLASS(Texture2d, Texture);

        Texture2d()           = default;
        ~Texture2d() override = default;

        Texture2d(TextureFlags flags, GfxTextureDesc desc);
    };

    WG_RTTI_CLASS_BEGIN(Texture2d) {
        WG_RTTI_META_DATA(RttiUiHint(""));
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

        TextureCube()           = default;
        ~TextureCube() override = default;

        TextureCube(TextureFlags flags, GfxTextureDesc desc);
    };

    WG_RTTI_CLASS_BEGIN(TextureCube) {
        WG_RTTI_META_DATA(RttiUiHint(""));
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge
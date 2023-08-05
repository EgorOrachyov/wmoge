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

#include "texture.hpp"

#include "core/class.hpp"
#include "core/data.hpp"
#include "core/engine.hpp"
#include "core/string_utils.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/yaml.hpp"
#include "resource/resource_manager.hpp"

#include <cassert>

namespace wmoge {

    bool yaml_read(const YamlConstNodeRef& node, TextureImportOptions& options) {
        WG_YAML_READ_AS_OPT(node, "channels", options.channels);
        WG_YAML_READ_AS_OPT(node, "format", options.format);
        WG_YAML_READ_AS_OPT(node, "mipmaps", options.mipmaps);
        WG_YAML_READ_AS_OPT(node, "srgb", options.srgb);
        WG_YAML_READ_AS_OPT(node, "compression", options.compression);
        WG_YAML_READ_AS_OPT(node, "sampling", options.sampling);
        WG_YAML_READ_AS_OPT(node, "compression", options.compression);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const TextureImportOptions& options) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "channels", options.channels);
        WG_YAML_WRITE_AS(node, "format", options.format);
        WG_YAML_WRITE_AS(node, "mipmaps", options.mipmaps);
        WG_YAML_WRITE_AS(node, "srgb", options.srgb);
        WG_YAML_WRITE_AS(node, "compression", options.compression);
        WG_YAML_WRITE_AS(node, "sampling", options.sampling);
        WG_YAML_WRITE_AS(node, "compression", options.compression);

        return true;
    }

    bool yaml_read(const YamlConstNodeRef& node, Texture2dImportOptions& options) {
        WG_YAML_READ_SUPER(node, TextureImportOptions, options);
        WG_YAML_READ_AS(node, "source_file", options.source_file);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const Texture2dImportOptions& options) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_SUPER(node, TextureImportOptions, options);
        WG_YAML_WRITE_AS(node, "source_file", options.source_file);

        return true;
    }
    bool yaml_read(const YamlConstNodeRef& node, TextureCubeImportOptions::SourceFiles& source_files) {
        WG_YAML_READ_AS(node, "right", source_files.right);
        WG_YAML_READ_AS(node, "left", source_files.left);
        WG_YAML_READ_AS(node, "top", source_files.top);
        WG_YAML_READ_AS(node, "bottom", source_files.bottom);
        WG_YAML_READ_AS(node, "back", source_files.back);
        WG_YAML_READ_AS(node, "front", source_files.front);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const TextureCubeImportOptions::SourceFiles& source_files) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "right", source_files.right);
        WG_YAML_WRITE_AS(node, "left", source_files.left);
        WG_YAML_WRITE_AS(node, "top", source_files.top);
        WG_YAML_WRITE_AS(node, "bottom", source_files.bottom);
        WG_YAML_WRITE_AS(node, "back", source_files.back);
        WG_YAML_WRITE_AS(node, "front", source_files.front);

        return true;
    }
    bool yaml_read(const YamlConstNodeRef& node, TextureCubeImportOptions& options) {
        WG_YAML_READ_SUPER(node, TextureImportOptions, options);
        WG_YAML_READ_AS(node, "source_files", options.source_files);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const TextureCubeImportOptions& options) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_SUPER(node, TextureImportOptions, options);
        WG_YAML_WRITE_AS(node, "source_files", options.source_files);

        return true;
    }

    Texture::Texture(GfxFormat format, int width, int height, int depth, int array_slices) {
        m_format       = format;
        m_width        = width;
        m_height       = height;
        m_depth        = depth;
        m_array_slices = array_slices;
        m_mips         = 1;
    }

    void Texture::set_source_images(std::vector<Ref<Image>> images) {
        m_images = std::move(images);
    }
    void Texture::set_sampler(const Ref<GfxSampler>& sampler) {
        m_sampler = sampler;
    }
    void Texture::set_sampler_from_desc(const GfxSamplerDesc& desc) {
        set_sampler(Engine::instance()->gfx_driver()->make_sampler(desc, SID(desc.to_str())));
    }
    void Texture::set_compression(const TexCompressionParams& params) {
        m_compression = params;
    }

    bool Texture::generate_mips() {
        WG_AUTO_PROFILE_RESOURCE("Texture::generate_mips");

        std::vector<Ref<Image>> mips;

        for (auto& image : m_images) {
            std::vector<Ref<Image>> face_mips;

            if (!image->generate_mip_chain(face_mips)) {
                WG_LOG_ERROR("failed to gen mip chain for texture " << get_name());
                return false;
            }

            for (auto& mip : face_mips) {
                mips.push_back(mip);
            }

            assert(m_mips == 1 || m_mips == int(face_mips.size()));
            m_mips = int(face_mips.size());
        }

        m_images = std::move(mips);
        return true;
    }
    bool Texture::generate_compressed_data() {
        WG_AUTO_PROFILE_RESOURCE("Texture::generate_compressed_data");

        if (m_compression.format == TexCompressionFormat::Unknown) {
            WG_LOG_INFO("no compression setup for texture " << get_name());
            return true;
        }
        if (m_images.empty()) {
            WG_LOG_INFO("no source to compress " << get_name());
            return true;
        }

        std::vector<GfxImageData> source_data;
        std::vector<GfxImageData> dest_data;

        source_data.reserve(m_images.size());

        for (auto& image : m_images) {
            GfxImageData data;
            data.format = m_format;
            data.depth  = 1;
            data.width  = image->get_width();
            data.height = image->get_height();
            data.data   = image->get_pixel_data();
            source_data.push_back(data);
        }

        if (!TexCompression::compress(m_compression, source_data, dest_data)) {
            WG_LOG_ERROR("failed to compress texture " << get_name());
            return false;
        }

        assert(source_data.size() == dest_data.size());

#ifdef WG_DEBUG
        std::size_t size_original   = 0;
        std::size_t size_compressed = 0;

        for (auto& entry : source_data) {
            size_original += entry.data->size();
        }
        for (auto& entry : dest_data) {
            size_compressed += entry.data->size();
        }

        WG_LOG_INFO("compressed texture "
                    << get_name()
                    << " dim=" << Vec3i(m_width, m_height, m_depth)
                    << " array=" << m_array_slices
                    << " fmt=" << magic_enum::enum_name(m_compression.format)
                    << " from=" << StringUtils::from_mem_size(size_original)
                    << " to=" << StringUtils::from_mem_size(size_compressed));
#endif

        m_format_compressed = dest_data.front().format;
        m_compressed        = std::move(dest_data);

        return true;
    }
    bool Texture::generate_gfx_resource() {
        WG_AUTO_PROFILE_RESOURCE("Texture::generate_gfx_resource");

        if (!m_sampler) {
            GfxSamplerDesc sampler_desc{};
            set_sampler_from_desc(sampler_desc);
        }

        GfxFormat format     = m_format;
        bool      compressed = false;

        if (!m_compressed.empty() && m_compression.format != TexCompressionFormat::Unknown) {
            format     = m_format_compressed;
            compressed = true;
        }

        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();

        switch (m_tex_type) {
            case GfxTex::Tex2d:
                m_texture = gfx_driver->make_texture_2d(m_width, m_height, m_mips, format, m_usages, m_mem_usage, get_name());
                break;
            case GfxTex::Tex2dArray:
                m_texture = gfx_driver->make_texture_2d_array(m_width, m_height, m_mips, m_array_slices, format, m_usages, m_mem_usage, get_name());
                break;
            case GfxTex::TexCube:
                m_texture = gfx_driver->make_texture_cube(m_width, m_height, m_mips, format, m_usages, m_mem_usage, get_name());
                break;
            default:
                WG_LOG_ERROR("unknown texture gfx type " << get_name());
                return false;
        }

        assert(m_depth == 1);
        assert(m_array_slices >= 1);
        assert(m_mips >= 1);

        for (int array_slice = 0; array_slice < m_array_slices; array_slice++) {
            for (int mip = 0; mip < m_mips; mip++) {
                const int index = array_slice * m_mips + mip;

                Ref<Data> data = m_images[index]->get_pixel_data();
                Rect2i    rect = {0, 0, m_images[index]->get_width(), m_images[index]->get_height()};

                if (compressed) {
                    data = m_compressed[index].data;
                    rect = {0, 0, m_compressed[index].width, m_compressed[index].height};
                }

                switch (m_tex_type) {
                    case GfxTex::Tex2d:
                        gfx_ctx->update_texture_2d(m_texture, mip, rect, data);
                        continue;
                    case GfxTex::Tex2dArray:
                        gfx_ctx->update_texture_2d_array(m_texture, mip, array_slice, rect, data);
                        continue;
                    case GfxTex::TexCube:
                        gfx_ctx->update_texture_cube(m_texture, mip, array_slice, rect, data);
                        continue;
                    default:
                        assert(false);
                }
            }
        }

        return true;
    }

    void Texture::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto texture            = dynamic_cast<Texture*>(&copy);
        texture->m_images       = m_images;
        texture->m_compressed   = m_compressed;
        texture->m_texture      = m_texture;
        texture->m_sampler      = m_sampler;
        texture->m_width        = m_width;
        texture->m_height       = m_height;
        texture->m_depth        = m_depth;
        texture->m_array_slices = m_array_slices;
        texture->m_mips         = m_mips;
        texture->m_tex_type     = m_tex_type;
        texture->m_mem_usage    = m_mem_usage;
        texture->m_usages       = m_usages;
        texture->m_srgb         = m_srgb;
        texture->m_compression  = m_compression;
    }
    void Texture::register_class() {
        auto* cls = Class::register_class<Texture>();
        cls->add_field(ClassField(VarType::Int, SID("width")), &Texture::m_width);
        cls->add_field(ClassField(VarType::Int, SID("height")), &Texture::m_height);
        cls->add_field(ClassField(VarType::Int, SID("depth")), &Texture::m_depth);
        cls->add_field(ClassField(VarType::Int, SID("array_slices")), &Texture::m_array_slices);
        cls->add_field(ClassField(VarType::Int, SID("mips")), &Texture::m_mips);
        cls->add_field(ClassField(VarType::Int, SID("srgb")), &Texture::m_srgb);
    }

    Texture2d::Texture2d(GfxFormat format, int width, int height) : Texture(format, width, height) {
        m_tex_type = GfxTex::Tex2d;
    }
    void Texture2d::copy_to(Resource& copy) {
        Texture::copy_to(copy);
    }
    void Texture2d::register_class() {
        auto* cls = Class::register_class<Texture2d>();
    }

    TextureCube::TextureCube(GfxFormat format, int width, int height) : Texture(format, width, height, 1, 6) {
        m_tex_type = GfxTex::TexCube;
    }
    void TextureCube::copy_to(Resource& copy) {
        Texture::copy_to(copy);
    }
    void TextureCube::register_class() {
        auto* cls = Class::register_class<TextureCube>();
    }

}// namespace wmoge
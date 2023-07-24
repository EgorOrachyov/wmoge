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
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "resource/resource_manager.hpp"

namespace wmoge {

    bool yaml_read(const YamlConstNodeRef& node, TextureImportOptions& options) {
        WG_YAML_READ_AS_OPT(node, "channels", options.channels);
        WG_YAML_READ_AS_OPT(node, "format", options.format);
        WG_YAML_READ_AS_OPT(node, "mipmaps", options.mipmaps);
        WG_YAML_READ_AS_OPT(node, "srgb", options.srgb);
        WG_YAML_READ_AS_OPT(node, "compression", options.compression);
        WG_YAML_READ_AS_OPT(node, "sampling", options.sampling);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const TextureImportOptions& options) {
        WG_YAML_WRITE_AS(node, "channels", options.channels);
        WG_YAML_WRITE_AS(node, "format", options.format);
        WG_YAML_WRITE_AS(node, "mipmaps", options.mipmaps);
        WG_YAML_WRITE_AS(node, "srgb", options.srgb);
        WG_YAML_WRITE_AS(node, "compression", options.compression);
        WG_YAML_WRITE_AS(node, "sampling", options.sampling);

        return true;
    }

    bool yaml_read(const YamlConstNodeRef& node, Texture2dImportOptions& options) {
        WG_YAML_READ_SUPER(node, TextureImportOptions, options);
        WG_YAML_READ_AS(node, "source_file", options.source_file);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const Texture2dImportOptions& options) {
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
        WG_YAML_WRITE_SUPER(node, TextureImportOptions, options);
        WG_YAML_WRITE_AS(node, "source_files", options.source_files);

        return true;
    }

    void Texture::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto texture            = dynamic_cast<Texture*>(&copy);
        texture->m_images       = m_images;
        texture->m_texture      = m_texture;
        texture->m_sampler      = m_sampler;
        texture->m_width        = m_width;
        texture->m_height       = m_height;
        texture->m_depth        = m_depth;
        texture->m_array_slices = m_array_slices;
        texture->m_mips         = m_mips;
        texture->m_tex_type     = m_tex_type;
        texture->m_format       = m_format;
        texture->m_mem_usage    = m_mem_usage;
        texture->m_usages       = m_usages;
        texture->m_srgb         = m_srgb;
        texture->m_compression  = m_compression;
    }
    void Texture::register_class() {
        auto* cls = Class::register_class<Texture>();
        cls->add_method(Method(VarType::Int, SID("get_width"), {}), &Texture::get_width, {});
        cls->add_method(Method(VarType::Int, SID("get_height"), {}), &Texture::get_height, {});
        cls->add_method(Method(VarType::Int, SID("get_depth"), {}), &Texture::get_depth, {});
        cls->add_method(Method(VarType::Int, SID("get_array_slices"), {}), &Texture::get_array_slices, {});
        cls->add_method(Method(VarType::Int, SID("get_mips"), {}), &Texture::get_mips, {});
        cls->add_method(Method(VarType::Int, SID("get_srgb"), {}), &Texture::get_srgb, {});
        cls->add_method(Method(VarType::Int, SID("get_compression"), {}), &Texture::get_compression, {});
    }

    void Texture2d::create(const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) {
        assert(texture);
        assert(sampler);

        m_texture      = texture;
        m_sampler      = sampler;
        m_width        = m_texture->width();
        m_height       = m_texture->height();
        m_depth        = m_texture->depth();
        m_array_slices = m_texture->array_slices();
        m_mips         = m_texture->mips_count();
        m_tex_type     = GfxTex::Tex2d;
        m_format       = m_texture->format();
        m_mem_usage    = m_texture->mem_usage();
    }
    void Texture2d::copy_to(Resource& copy) {
        Texture::copy_to(copy);
    }
    void Texture2d::register_class() {
        auto* cls = Class::register_class<Texture2d>();
    }

    void TextureCube::create(const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) {
        assert(texture);
        assert(sampler);

        m_texture      = texture;
        m_sampler      = sampler;
        m_width        = m_texture->width();
        m_height       = m_texture->height();
        m_depth        = m_texture->depth();
        m_array_slices = m_texture->array_slices();
        m_mips         = m_texture->mips_count();
        m_tex_type     = GfxTex::TexCube;
        m_format       = m_texture->format();
        m_mem_usage    = m_texture->mem_usage();
    }
    void TextureCube::copy_to(Resource& copy) {
        Texture::copy_to(copy);
    }
    void TextureCube::register_class() {
        auto* cls = Class::register_class<TextureCube>();
    }

}// namespace wmoge
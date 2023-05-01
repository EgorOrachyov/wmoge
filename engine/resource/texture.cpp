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
#include "resource/resource_manager.hpp"

#include <magic_enum.hpp>
#include <sstream>

namespace wmoge {

    void Texture::load_sampler_from_import_options(const YamlTree& tree) {
        auto sampler = tree["params"]["sampling"];

        float       min_lod;
        float       max_lod;
        float       max_anisotropy;
        std::string min_flt;
        std::string mag_flt;
        std::string u;
        std::string v;
        std::string w;
        std::string brd_clr;

        sampler["min_lod"] >> min_lod;
        sampler["max_lod"] >> max_lod;
        sampler["max_anisotropy"] >> max_anisotropy;
        sampler["min_flt"] >> min_flt;
        sampler["mag_flt"] >> mag_flt;
        sampler["u"] >> u;
        sampler["v"] >> v;
        sampler["w"] >> w;
        sampler["brd_clr"] >> brd_clr;

        GfxSamplerDesc desc;
        desc.min_lod        = min_lod;
        desc.max_lod        = max_lod;
        desc.max_anisotropy = max_anisotropy;
        desc.min_flt        = magic_enum::enum_cast<GfxSampFlt>(min_flt).value();
        desc.mag_flt        = magic_enum::enum_cast<GfxSampFlt>(mag_flt).value();
        desc.u              = magic_enum::enum_cast<GfxSampAddress>(u).value();
        desc.v              = magic_enum::enum_cast<GfxSampAddress>(v).value();
        desc.w              = magic_enum::enum_cast<GfxSampAddress>(w).value();
        desc.brd_clr        = magic_enum::enum_cast<GfxSampBrdClr>(brd_clr).value();

        std::stringstream sampler_name;
        sampler_name << min_flt << ":" << mag_flt << ","
                     << u << ":" << v << ":" << w << ","
                     << min_lod << ":" << max_lod << ","
                     << max_anisotropy;

        m_sampler = Engine::instance()->gfx_driver()->make_sampler(desc, SID(sampler_name.str()));
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

    void Texture2d::create(const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) {
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
    bool Texture2d::load_from_import_options(const YamlTree& tree) {
        WG_AUTO_PROFILE_RESOURCE();

        load_sampler_from_import_options(tree);

        bool        mipmaps  = false;
        int         channels = 4;
        std::string source_file;
        std::string format;

        auto params = tree["params"];
        params["channels"] >> channels;
        params["source_file"] >> source_file;
        params["format"] >> format;
        params["mipmaps"] >> mipmaps;
        params["compression"] >> m_compression;
        params["srgb"] >> m_srgb;

        Image source;
        if (!source.load(source_file, channels)) return false;

        if (mipmaps) {
            if (!source.generate_mip_chain(m_images)) return false;
        } else {
            m_images.push_back(source.duplicate().cast<Image>());
        }

        m_width        = source.get_width();
        m_height       = source.get_height();
        m_depth        = 1;
        m_array_slices = 1;
        m_mips         = static_cast<int>(m_images.size());
        m_tex_type     = GfxTex::Tex2d;
        m_mem_usage    = GfxMemUsage::GpuLocal;
        m_usages.set(GfxTexUsageFlag::Sampling);
        m_format = magic_enum::enum_cast<GfxFormat>(format).value();

        auto* gfx = Engine::instance()->gfx_driver();
        m_texture = gfx->make_texture_2d(m_width, m_height, m_mips, m_format, m_usages, m_mem_usage, get_name());
        for (int i = 0; i < m_mips; i++) {
            auto& mip = m_images[i];
            gfx->update_texture_2d(m_texture, i, Rect2i(0, 0, mip->get_width(), mip->get_height()), mip->get_pixel_data());
        }

        return true;
    }
    void Texture2d::copy_to(Resource& copy) {
        Texture::copy_to(copy);
    }
    void Texture2d::register_class() {
        auto* cls = Class::register_class<Texture2d>();
    }

    bool TextureCube::load_from_import_options(const YamlTree& tree) {
        WG_AUTO_PROFILE_RESOURCE();

        load_sampler_from_import_options(tree);

        bool        mipmaps  = false;
        int         channels = 4;
        std::string sides[6];
        std::string format;

        auto params = tree["params"];
        params["channels"] >> channels;
        params["format"] >> format;
        params["mipmaps"] >> mipmaps;
        params["compression"] >> m_compression;
        params["srgb"] >> m_srgb;

        auto source_files = params["source_files"];
        source_files["right"] >> sides[0];
        source_files["left"] >> sides[1];
        source_files["top"] >> sides[2];
        source_files["bottom"] >> sides[3];
        source_files["back"] >> sides[4];
        source_files["front"] >> sides[5];

        for (int i = 0; i < 6; i++) {
            Image side;
            if (!side.load(sides[i], channels)) return false;
            if (mipmaps) {
                if (!side.generate_mip_chain(m_images)) return false;
            } else {
                m_images.push_back(side.duplicate().cast<Image>());
            }
            m_width  = side.get_width();
            m_height = side.get_height();
        }

        m_depth        = 1;
        m_array_slices = 6;
        m_mips         = static_cast<int>(m_images.size()) / m_array_slices;
        m_tex_type     = GfxTex::TexCube;
        m_mem_usage    = GfxMemUsage::GpuLocal;
        m_usages.set(GfxTexUsageFlag::Sampling);
        m_format = magic_enum::enum_cast<GfxFormat>(format).value();

        auto* gfx = Engine::instance()->gfx_driver();
        m_texture = gfx->make_texture_cube(m_width, m_height, m_mips, m_format, m_usages, m_mem_usage, get_name());

        for (int f = 0; f < 6; f++) {
            for (int i = 0; i < m_mips; i++) {
                auto& mip = m_images[f * m_mips + i];
                gfx->update_texture_cube(m_texture, i, f, Rect2i(0, 0, mip->get_width(), mip->get_height()), mip->get_pixel_data());
            }
        }

        return true;
    }
    void TextureCube::copy_to(Resource& copy) {
        Texture::copy_to(copy);
    }
    void TextureCube::register_class() {
        auto* cls = Class::register_class<TextureCube>();
    }

}// namespace wmoge
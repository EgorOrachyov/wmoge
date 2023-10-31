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

#include "graphics_pipeline.hpp"

#include "core/engine.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, BloomSettings& settings) {
        WG_YAML_READ_AS_OPT(node, "enable", settings.enable);
        WG_YAML_READ_AS_OPT(node, "intensity", settings.intensity);
        WG_YAML_READ_AS_OPT(node, "threshold", settings.threshold);
        WG_YAML_READ_AS_OPT(node, "knee", settings.knee);
        WG_YAML_READ_AS_OPT(node, "radius", settings.radius);
        WG_YAML_READ_AS_OPT(node, "uspample_weight", settings.uspample_weight);
        WG_YAML_READ_AS_OPT(node, "dirt_mask_intensity", settings.dirt_mask_intensity);
        WG_YAML_READ_AS_OPT(node, "dirt_mask", settings.dirt_mask);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const BloomSettings& settings) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "enable", settings.enable);
        WG_YAML_WRITE_AS(node, "intensity", settings.intensity);
        WG_YAML_WRITE_AS(node, "threshold", settings.threshold);
        WG_YAML_WRITE_AS(node, "knee", settings.knee);
        WG_YAML_WRITE_AS(node, "radius", settings.radius);
        WG_YAML_WRITE_AS(node, "uspample_weight", settings.uspample_weight);
        WG_YAML_WRITE_AS(node, "dirt_mask_intensity", settings.dirt_mask_intensity);
        WG_YAML_WRITE_AS(node, "dirt_mask", settings.dirt_mask);
        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, TonemapSettings& settings) {
        WG_YAML_READ_AS_OPT(node, "exposure", settings.exposure);
        WG_YAML_READ_AS_OPT(node, "white_point", settings.white_point);
        WG_YAML_READ_AS_OPT(node, "mode", settings.mode);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const TonemapSettings& settings) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "exposure", settings.exposure);
        WG_YAML_WRITE_AS(node, "white_point", settings.white_point);
        WG_YAML_WRITE_AS(node, "mode", settings.mode);
        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, GraphicsPipelineSettings& settings) {
        WG_YAML_READ_AS_OPT(node, "bloom", settings.bloom);
        WG_YAML_READ_AS_OPT(node, "tonemap", settings.tonemap);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const GraphicsPipelineSettings& settings) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "bloom", settings.bloom);
        WG_YAML_WRITE_AS(node, "tonemap", settings.tonemap);
        return StatusCode::Ok;
    }

    void GraphicsPipelineTextures::resize(Size2i new_target_resoulution) {
        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();

        const Size2i       size         = new_target_resoulution;
        const GfxTexUsages usages       = {GfxTexUsageFlag::ColorTarget, GfxTexUsageFlag::Sampling, GfxTexUsageFlag::Storage};
        const GfxTexUsages depth_usages = {GfxTexUsageFlag::DepthTarget, GfxTexUsageFlag::Sampling};

        depth        = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::DEPTH32F, depth_usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("depth"));
        primitive_id = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::R32I, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("primitive_id"));
        gbuffer[0]   = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("gbuffer[0]"));
        gbuffer[1]   = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("gbuffer[1]"));
        gbuffer[2]   = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("gbuffer[2]"));
        bloom_downsample.clear();
        bloom_upsample.clear();

        const int mips       = Image::max_mips_count(size.x(), size.y(), 1);
        const int mip_bias   = 3;
        const int bloom_mips = Math::max(int(0), mips - mip_bias);

        for (int i = 0; i < bloom_mips; i++) {
            const Size2i   mip_size = Image::mip_size(i, size.x(), size.y());
            const StringId name     = SID("bloom mip=" + StringUtils::from_int(i));

            bloom_downsample.push_back(gfx_driver->make_texture_2d(mip_size.x(), mip_size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, name));
            bloom_upsample.push_back(gfx_driver->make_texture_2d(mip_size.x(), mip_size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, name));
        }

        color_hdr = gbuffer[0];//tmp
        color_ldr = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA8, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("color_ldr"));

        target_viewport = Rect2i(0, 0, new_target_resoulution.x(), new_target_resoulution.y());
        target_size     = Vec2u(new_target_resoulution.x(), new_target_resoulution.y());
    }

    void GraphicsPipelineTextures::update_viewport(Size2i new_resoulution) {
        viewport = Rect2i(0, 0, new_resoulution.x(), new_resoulution.y());
        size     = Vec2u(new_resoulution.x(), new_resoulution.y());
    }

    GraphicsPipelineStage::GraphicsPipelineStage() {
        Engine* engine = Engine::instance();

        m_gfx_driver     = engine->gfx_driver();
        m_gfx_ctx        = engine->gfx_ctx();
        m_shader_manager = engine->shader_manager();
        m_tex_manager    = engine->texture_manager();
        m_render_engine  = engine->render_engine();
    }

    void GraphicsPipelineStage::set_pipeline(GraphicsPipeline* pipeline) {
        m_pipeline = pipeline;
    }

    void GraphicsPipeline::set_scene(RenderScene* scene) {
        m_scene = scene;
    }
    void GraphicsPipeline::set_cameras(RenderCameras* cameras) {
        m_cameras = cameras;
    }
    void GraphicsPipeline::set_views(ArrayView<struct RenderView> views) {
        m_views = views;
    }

    void GraphicsPipeline::set_target_resolution(Size2i resolution) {
        m_target_resolution = resolution;
        m_textures.resize(m_target_resolution);
        m_textures.update_viewport(m_resolution);
    }

    void GraphicsPipeline::set_resolution(Size2i resolution) {
        m_resolution = resolution;
        m_textures.update_viewport(m_resolution);
    }

    void GraphicsPipeline::set_settings(const GraphicsPipelineSettings& settings) {
        m_settings = settings;
    }

}// namespace wmoge

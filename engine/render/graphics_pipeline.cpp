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

    void GraphicsPipelineTextures::resize(Size2i new_target_resoulution) {
        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();

        const Size2i       size         = new_target_resoulution;
        const GfxTexUsages usages       = {GfxTexUsageFlag::ColorTarget, GfxTexUsageFlag::Sampling};
        const GfxTexUsages depth_usages = {GfxTexUsageFlag::DepthTarget, GfxTexUsageFlag::Sampling};

        depth        = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::DEPTH32F, depth_usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("depth"));
        primitive_id = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::R32I, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("primitive_id"));
        gbuffer[0]   = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("gbuffer[0]"));
        gbuffer[1]   = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("gbuffer[1]"));
        gbuffer[2]   = gfx_driver->make_texture_2d(size.x(), size.y(), 1, GfxFormat::RGBA16F, usages, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("gbuffer[2]"));

        target_viewport = Rect2i(0, 0, new_target_resoulution.x(), new_target_resoulution.y());
    }

    void GraphicsPipelineTextures::update_viewport(Size2i new_resoulution) {
        viewport = Rect2i(0, 0, new_resoulution.x(), new_resoulution.y());
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

}// namespace wmoge

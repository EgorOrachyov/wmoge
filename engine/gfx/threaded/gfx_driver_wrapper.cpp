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

#include "gfx_driver_wrapper.hpp"

#include "core/log.hpp"
#include "profiler/profiler.hpp"

#include <cassert>

namespace wmoge {

    GfxDriverWrapper::GfxDriverWrapper(GfxDriverThreaded* driver) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::GfxDriverWrapper");

        assert(driver);

        m_driver              = driver;
        m_stream              = driver->cmd_stream();
        m_shader_lang         = driver->shader_lang();
        m_device_caps         = driver->device_caps();
        m_driver_name         = driver->driver_name();
        m_thread_id           = driver->thread_id();
        m_clip_matrix         = driver->clip_matrix();
        m_pipeline_cache_path = driver->pipeline_cache_path();
        m_ctx_immediate       = driver->ctx_immediate();
        m_ctx_async           = driver->ctx_async();
        m_gfx_type            = driver->get_gfx_type();
        m_shader_platform     = driver->get_shader_platform();
    }

    Ref<GfxVertFormat> GfxDriverWrapper::make_vert_format(const GfxVertElements& elements, const Strid& name) {
        Ref<GfxVertFormat> vert_format;
        m_stream->push_and_wait([&]() { vert_format = m_driver->make_vert_format(elements, name); });
        return vert_format;
    }
    Ref<GfxVertBuffer> GfxDriverWrapper::make_vert_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_vert_buffer");

        Ref<GfxVertBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_vert_buffer(size, usage, name); });
        return buffer;
    }
    Ref<GfxIndexBuffer> GfxDriverWrapper::make_index_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_index_buffer");

        Ref<GfxIndexBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_index_buffer(size, usage, name); });
        return buffer;
    }
    Ref<GfxUniformBuffer> GfxDriverWrapper::make_uniform_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_uniform_buffer");

        Ref<GfxUniformBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_uniform_buffer(size, usage, name); });
        return buffer;
    }
    Ref<GfxStorageBuffer> GfxDriverWrapper::make_storage_buffer(int size, GfxMemUsage usage, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_storage_buffer");

        Ref<GfxStorageBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_storage_buffer(size, usage, name); });
        return buffer;
    }
    Ref<GfxShader> GfxDriverWrapper::make_shader(GfxShaderDesc desc, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_shader");

        Ref<GfxShader> shader;
        m_stream->push_and_wait([&]() { shader = m_driver->make_shader(std::move(desc), name); });
        return shader;
    }
    Ref<GfxShaderProgram> GfxDriverWrapper::make_program(GfxShaderProgramDesc desc, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_program");

        Ref<GfxShaderProgram> program;
        m_stream->push_and_wait([&]() { program = m_driver->make_program(std::move(desc), name); });
        return program;
    }
    Ref<GfxTexture> GfxDriverWrapper::make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, GfxTexSwizz swizz, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_texture_2d");

        Ref<GfxTexture> texture;
        m_stream->push_and_wait([&]() { texture = m_driver->make_texture_2d(width, height, mips, format, usages, mem_usage, swizz, name); });
        return texture;
    }
    Ref<GfxTexture> GfxDriverWrapper::make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_texture_2d_array");

        Ref<GfxTexture> texture;
        m_stream->push_and_wait([&]() { texture = m_driver->make_texture_2d_array(width, height, mips, slices, format, usages, mem_usage, name); });
        return texture;
    }
    Ref<GfxTexture> GfxDriverWrapper::make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_texture_cube");

        Ref<GfxTexture> texture;
        m_stream->push_and_wait([&]() { texture = m_driver->make_texture_cube(width, height, mips, format, usages, mem_usage, name); });
        return texture;
    }
    Ref<GfxSampler> GfxDriverWrapper::make_sampler(const GfxSamplerDesc& desc, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_sampler");

        Ref<GfxSampler> sampler;
        m_stream->push_and_wait([&]() { sampler = m_driver->make_sampler(desc, name); });
        return sampler;
    }
    Ref<GfxPsoLayout> GfxDriverWrapper::make_pso_layout(const GfxDescSetLayouts& layouts, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_pso_layout");

        Ref<GfxPsoLayout> pipeline_layout;
        m_stream->push_and_wait([&]() { pipeline_layout = m_driver->make_pso_layout(layouts, name); });
        return pipeline_layout;
    }
    Ref<GfxPsoGraphics> GfxDriverWrapper::make_pso_graphics(const GfxPsoStateGraphics& state, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_pso_graphics");

        Ref<GfxPsoGraphics> pipeline;
        m_stream->push_and_wait([&]() { pipeline = m_driver->make_pso_graphics(state, name); });
        return pipeline;
    }
    Ref<GfxPsoCompute> GfxDriverWrapper::make_pso_compute(const GfxPsoStateCompute& state, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_pso_compute");

        Ref<GfxPsoCompute> pipeline;
        m_stream->push_and_wait([&]() { pipeline = m_driver->make_pso_compute(state, name); });
        return pipeline;
    }
    Ref<GfxRenderPass> GfxDriverWrapper::make_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_render_pass");

        Ref<GfxRenderPass> render_pass;
        m_stream->push_and_wait([&]() { render_pass = m_driver->make_render_pass(pass_desc, name); });
        return render_pass;
    }
    Ref<GfxDynVertBuffer> GfxDriverWrapper::make_dyn_vert_buffer(int chunk_size, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_dyn_vert_buffer");

        Ref<GfxDynVertBuffer> dyn_buffer;
        m_stream->push_and_wait([&]() { dyn_buffer = m_driver->make_dyn_vert_buffer(chunk_size, name); });
        return dyn_buffer;
    }
    Ref<GfxDynIndexBuffer> GfxDriverWrapper::make_dyn_index_buffer(int chunk_size, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_dyn_index_buffer");

        Ref<GfxDynIndexBuffer> dyn_buffer;
        m_stream->push_and_wait([&]() { dyn_buffer = m_driver->make_dyn_index_buffer(chunk_size, name); });
        return dyn_buffer;
    }
    Ref<GfxDynUniformBuffer> GfxDriverWrapper::make_dyn_uniform_buffer(int chunk_size, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_dyn_uniform_buffer");

        Ref<GfxDynUniformBuffer> dyn_buffer;
        m_stream->push_and_wait([&]() { dyn_buffer = m_driver->make_dyn_uniform_buffer(chunk_size, name); });
        return dyn_buffer;
    }
    Ref<GfxDescSetLayout> GfxDriverWrapper::make_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_desc_layout");

        Ref<GfxDescSetLayout> desc_layout;
        m_stream->push_and_wait([&]() { desc_layout = m_driver->make_desc_layout(desc, name); });
        return desc_layout;
    }
    Ref<GfxDescSet> GfxDriverWrapper::make_desc_set(const GfxDescSetResources& resources, const Ref<GfxDescSetLayout>& layout, const Strid& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_desc_set");

        Ref<GfxDescSet> desc_set;
        m_stream->push_and_wait([&]() { desc_set = m_driver->make_desc_set(resources, layout, name); });
        return desc_set;
    }
    Async GfxDriverWrapper::make_shaders(const Ref<GfxAsyncShaderRequest>& request) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_shaders");

        Async result;
        m_stream->push_and_wait([&]() { result = m_driver->make_shaders(request); });
        return result;
    }
    Async GfxDriverWrapper::make_psos_graphics(const Ref<GfxAsyncPsoRequestGraphics>& request) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_psos_graphics");

        Async result;
        m_stream->push_and_wait([&]() { result = m_driver->make_psos_graphics(request); });
        return result;
    }
    Async GfxDriverWrapper::make_psos_compute(const Ref<GfxAsyncPsoRequestCompute>& request) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_psos_compute");

        Async result;
        m_stream->push_and_wait([&]() { result = m_driver->make_psos_compute(request); });
        return result;
    }

    void GfxDriverWrapper::shutdown() {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::shutdown");

        m_stream->push_and_wait([=]() { m_driver->shutdown(); });
    }

    void GfxDriverWrapper::begin_frame() {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::begin_frame");

        m_stream->push([=]() { m_driver->begin_frame(); });
    }
    void GfxDriverWrapper::end_frame() {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::end_frame");

        m_stream->push([=]() { m_driver->end_frame(); });
    }
    void GfxDriverWrapper::prepare_window(const Ref<Window>& window) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::prepare_window");

        m_stream->push([=]() { m_driver->prepare_window(window); });
    }
    void GfxDriverWrapper::swap_buffers(const Ref<Window>& window) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::swap_buffers");

        m_stream->push_and_wait([=]() { m_driver->swap_buffers(window); });
    }

    class GfxCtx* GfxDriverWrapper::ctx_immediate() {
        return m_ctx_immediate;
    }
    class GfxCtx* GfxDriverWrapper::ctx_async() {
        return m_ctx_async;
    }

    const GfxDeviceCaps& GfxDriverWrapper::device_caps() const {
        return m_device_caps;
    }
    const Strid& GfxDriverWrapper::driver_name() const {
        return m_driver_name;
    }
    const std::string& GfxDriverWrapper::pipeline_cache_path() const {
        return m_pipeline_cache_path;
    }
    const std::thread::id& GfxDriverWrapper::thread_id() const {
        return m_thread_id;
    }
    const Mat4x4f& GfxDriverWrapper::clip_matrix() const {
        return m_clip_matrix;
    }
    size_t GfxDriverWrapper::frame_number() const {
        return m_driver->frame_number();
    }
    bool GfxDriverWrapper::on_gfx_thread() const {
        return m_thread_id == std::this_thread::get_id();
    }
    GfxShaderLang GfxDriverWrapper::shader_lang() const {
        return m_shader_lang;
    }
    GfxType GfxDriverWrapper::get_gfx_type() const {
        return m_gfx_type;
    }
    GfxShaderPlatform GfxDriverWrapper::get_shader_platform() const {
        return m_shader_platform;
    }

}// namespace wmoge
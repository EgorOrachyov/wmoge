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

#include "debug/profiler.hpp"

#include <cassert>

namespace wmoge {

    GfxDriverWrapper::GfxDriverWrapper(GfxDriverThreaded* driver) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::GfxDriverWrapper");

        assert(driver);

        m_driver              = driver;
        m_stream              = driver->cmd_stream();
        m_device_caps         = driver->device_caps();
        m_driver_name         = driver->driver_name();
        m_thread_id           = driver->thread_id();
        m_clip_matrix         = driver->clip_matrix();
        m_shader_cache_path   = driver->shader_cache_path();
        m_pipeline_cache_path = driver->pipeline_cache_path();
    }

    ref_ptr<GfxVertFormat> GfxDriverWrapper::make_vert_format(const GfxVertElements& elements, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_vert_format");

        ref_ptr<GfxVertFormat> vert_format;
        m_stream->push_and_wait([&]() { vert_format = m_driver->make_vert_format(elements, name); });
        return vert_format;
    }
    ref_ptr<GfxVertBuffer> GfxDriverWrapper::make_vert_buffer(int size, GfxMemUsage usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_vert_buffer");

        ref_ptr<GfxVertBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_vert_buffer(size, usage, name); });
        return buffer;
    }
    ref_ptr<GfxIndexBuffer> GfxDriverWrapper::make_index_buffer(int size, GfxMemUsage usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_index_buffer");

        ref_ptr<GfxIndexBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_index_buffer(size, usage, name); });
        return buffer;
    }
    ref_ptr<GfxUniformBuffer> GfxDriverWrapper::make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_uniform_buffer");

        ref_ptr<GfxUniformBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_uniform_buffer(size, usage, name); });
        return buffer;
    }
    ref_ptr<GfxStorageBuffer> GfxDriverWrapper::make_storage_buffer(int size, GfxMemUsage usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_storage_buffer");

        ref_ptr<GfxStorageBuffer> buffer;
        m_stream->push_and_wait([&]() { buffer = m_driver->make_storage_buffer(size, usage, name); });
        return buffer;
    }
    ref_ptr<GfxShader> GfxDriverWrapper::make_shader(std::string vertex, std::string fragment, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_shader");

        ref_ptr<GfxShader> shader;
        m_stream->push_and_wait([&]() { shader = m_driver->make_shader(std::move(vertex), std::move(fragment), name); });
        return shader;
    }
    ref_ptr<GfxShader> GfxDriverWrapper::make_shader(ref_ptr<Data> code, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_shader");

        ref_ptr<GfxShader> shader;
        m_stream->push_and_wait([&]() { shader = m_driver->make_shader(std::move(code), name); });
        return shader;
    }
    ref_ptr<GfxTexture> GfxDriverWrapper::make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_texture_2d");

        ref_ptr<GfxTexture> texture;
        m_stream->push_and_wait([&]() { texture = m_driver->make_texture_2d(width, height, mips, format, usages, mem_usage, name); });
        return texture;
    }
    ref_ptr<GfxTexture> GfxDriverWrapper::make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_texture_2d_array");

        ref_ptr<GfxTexture> texture;
        m_stream->push_and_wait([&]() { texture = m_driver->make_texture_2d_array(width, height, mips, slices, format, usages, mem_usage, name); });
        return texture;
    }
    ref_ptr<GfxTexture> GfxDriverWrapper::make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_texture_cube");

        ref_ptr<GfxTexture> texture;
        m_stream->push_and_wait([&]() { texture = m_driver->make_texture_cube(width, height, mips, format, usages, mem_usage, name); });
        return texture;
    }
    ref_ptr<GfxSampler> GfxDriverWrapper::make_sampler(const GfxSamplerDesc& desc, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_sampler");

        ref_ptr<GfxSampler> sampler;
        m_stream->push_and_wait([&]() { sampler = m_driver->make_sampler(desc, name); });
        return sampler;
    }
    ref_ptr<GfxRenderPass> GfxDriverWrapper::make_render_pass(GfxRenderPassType pass_type, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_render_pass");

        ref_ptr<GfxRenderPass> render_pass;
        m_stream->push_and_wait([&]() { render_pass = m_driver->make_render_pass(pass_type, name); });
        return render_pass;
    }
    ref_ptr<GfxPipeline> GfxDriverWrapper::make_pipeline(const GfxPipelineState& state, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::make_pipeline");

        ref_ptr<GfxPipeline> pipeline;
        m_stream->push_and_wait([&]() { pipeline = m_driver->make_pipeline(state, name); });
        return pipeline;
    }

    void GfxDriverWrapper::update_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_vert_buffer");

        m_stream->push([=]() { m_driver->update_vert_buffer(buffer, offset, range, data); });
    }
    void GfxDriverWrapper::update_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_index_buffer");

        m_stream->push([=]() { m_driver->update_index_buffer(buffer, offset, range, data); });
    }
    void GfxDriverWrapper::update_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_uniform_buffer");

        m_stream->push([=]() { m_driver->update_uniform_buffer(buffer, offset, range, data); });
    }
    void GfxDriverWrapper::update_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer, int offset, int range, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_storage_buffer");

        m_stream->push([=]() { m_driver->update_storage_buffer(buffer, offset, range, data); });
    }
    void GfxDriverWrapper::update_texture_2d(const ref_ptr<GfxTexture>& texture, int mip, Rect2i region, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_texture_2d");

        m_stream->push([=]() { m_driver->update_texture_2d(texture, mip, region, data); });
    }
    void GfxDriverWrapper::update_texture_2d_array(const ref_ptr<GfxTexture>& texture, int mip, int slice, Rect2i region, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_texture_2d_array");

        m_stream->push([=]() { m_driver->update_texture_2d_array(texture, mip, slice, region, data); });
    }
    void GfxDriverWrapper::update_texture_cube(const ref_ptr<GfxTexture>& texture, int mip, int face, Rect2i region, const ref_ptr<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::update_texture_cube");

        m_stream->push([=]() { m_driver->update_texture_cube(texture, mip, face, region, data); });
    }

    void* GfxDriverWrapper::map_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::map_vert_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_driver->map_vert_buffer(buffer); });
        return mapped_ptr;
    }
    void* GfxDriverWrapper::map_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::map_index_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_driver->map_index_buffer(buffer); });
        return mapped_ptr;
    }
    void* GfxDriverWrapper::map_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::map_uniform_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_driver->map_uniform_buffer(buffer); });
        return mapped_ptr;
    }
    void* GfxDriverWrapper::map_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::map_storage_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_driver->map_storage_buffer(buffer); });
        return mapped_ptr;
    }
    void GfxDriverWrapper::unmap_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::unmap_vert_buffer");

        m_stream->push([=]() { m_driver->unmap_vert_buffer(buffer); });
    }
    void GfxDriverWrapper::unmap_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::unmap_index_buffer");

        m_stream->push([=]() { m_driver->unmap_index_buffer(buffer); });
    }
    void GfxDriverWrapper::unmap_uniform_buffer(const ref_ptr<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::unmap_uniform_buffer");

        m_stream->push([=]() { m_driver->unmap_uniform_buffer(buffer); });
    }
    void GfxDriverWrapper::unmap_storage_buffer(const ref_ptr<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::unmap_storage_buffer");

        m_stream->push([=]() { m_driver->unmap_storage_buffer(buffer); });
    }

    void GfxDriverWrapper::begin_render_pass(const ref_ptr<GfxRenderPass>& pass) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::begin_render_pass");

        m_stream->push([=]() { m_driver->begin_render_pass(pass); });
    }
    void GfxDriverWrapper::bind_target(const ref_ptr<Window>& window) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_target");

        m_stream->push([=]() { m_driver->bind_target(window); });
    }
    void GfxDriverWrapper::bind_color_target(const ref_ptr<GfxTexture>& texture, int target, int mip, int slice) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_color_target");

        m_stream->push([=]() { m_driver->bind_color_target(texture, target, mip, slice); });
    }
    void GfxDriverWrapper::bind_depth_target(const ref_ptr<GfxTexture>& texture, int mip, int slice) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_depth_target");

        m_stream->push([=]() { m_driver->bind_depth_target(texture, mip, slice); });
    }
    void GfxDriverWrapper::viewport(const Rect2i& viewport) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::viewport");

        m_stream->push([=]() { m_driver->viewport(viewport); });
    }
    void GfxDriverWrapper::clear(int target, const Vec4f& color) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::clear");

        m_stream->push([=]() { m_driver->clear(target, color); });
    }
    void GfxDriverWrapper::clear(float depth, int stencil) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::clear");

        m_stream->push([=]() { m_driver->clear(depth, stencil); });
    }
    bool GfxDriverWrapper::bind_pipeline(const ref_ptr<GfxPipeline>& pipeline) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_pipeline");

        bool is_bound = false;
        m_stream->push_and_wait([&]() { is_bound = m_driver->bind_pipeline(pipeline); });
        return is_bound;
    }
    void GfxDriverWrapper::bind_vert_buffer(const ref_ptr<GfxVertBuffer>& buffer, int index, int offset) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_vert_buffer");

        m_stream->push([=]() { m_driver->bind_vert_buffer(buffer, index, offset); });
    }
    void GfxDriverWrapper::bind_index_buffer(const ref_ptr<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_index_buffer");

        m_stream->push([=]() { m_driver->bind_index_buffer(buffer, index_type, offset); });
    }
    void GfxDriverWrapper::bind_texture(const StringId& name, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_texture");

        m_stream->push([=]() { m_driver->bind_texture(name, array_element, texture, sampler); });
    }
    void GfxDriverWrapper::bind_texture(const GfxLocation& location, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_texture");

        m_stream->push([=]() { m_driver->bind_texture(location, array_element, texture, sampler); });
    }
    void GfxDriverWrapper::bind_uniform_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_uniform_buffer");

        m_stream->push([=]() { m_driver->bind_uniform_buffer(name, offset, range, buffer); });
    }
    void GfxDriverWrapper::bind_uniform_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_uniform_buffer");

        m_stream->push([=]() { m_driver->bind_uniform_buffer(location, offset, range, buffer); });
    }
    void GfxDriverWrapper::bind_storage_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_storage_buffer");

        m_stream->push([=]() { m_driver->bind_storage_buffer(name, offset, range, buffer); });
    }
    void GfxDriverWrapper::bind_storage_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::bind_storage_buffer");

        m_stream->push([=]() { m_driver->bind_storage_buffer(location, offset, range, buffer); });
    }
    void GfxDriverWrapper::draw(int vertex_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::draw");

        m_stream->push([=]() { m_driver->draw(vertex_count, base_vertex, instance_count); });
    }
    void GfxDriverWrapper::draw_indexed(int index_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::draw_indexed");

        m_stream->push([=]() { m_driver->draw_indexed(index_count, base_vertex, instance_count); });
    }
    void GfxDriverWrapper::end_render_pass() {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::end_render_pass");

        m_stream->push([=]() { m_driver->end_render_pass(); });
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
    void GfxDriverWrapper::prepare_window(const ref_ptr<Window>& window) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::prepare_window");

        m_stream->push([=]() { m_driver->prepare_window(window); });
    }
    void GfxDriverWrapper::swap_buffers(const ref_ptr<Window>& window) {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::swap_buffers");

        m_stream->push([=]() { m_driver->swap_buffers(window); });
    }
    void GfxDriverWrapper::flush() {
        WG_AUTO_PROFILE_GFX("GfxDriverWrapper::flush");

        m_stream->push_and_wait([=]() { m_driver->flush(); });
    }

    const GfxDeviceCaps& GfxDriverWrapper::device_caps() const {
        return m_device_caps;
    }
    const StringId& GfxDriverWrapper::driver_name() const {
        return m_driver_name;
    }
    const std::string& GfxDriverWrapper::shader_cache_path() const {
        return m_shader_cache_path;
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

}// namespace wmoge
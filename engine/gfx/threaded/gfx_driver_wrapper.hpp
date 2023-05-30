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

#ifndef WMOGE_GFX_DRIVER_WRAPPER_HPP
#define WMOGE_GFX_DRIVER_WRAPPER_HPP

#include "core/cmd_stream.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/threaded/gfx_driver_threaded.hpp"

namespace wmoge {

    /**
     * @class GfxDriverWrapper
     * @brief Thread-safe wrapper for gfx device to used from any thread
     *
     * Wraps GfxDriver interface. It uses commands serialization to send it
     * to a separate gfx thread, responsible for GPU communication. Simple
     * commands sent with no wait. Commands requiring immediate feedback
     * require to wait until gfx thread process them.
     *
     * @see GfxDriver
     * @see GfxDriverThreaded
     */
    class GfxDriverWrapper : public GfxDriver {
    public:
        explicit GfxDriverWrapper(GfxDriverThreaded* driver);

        ~GfxDriverWrapper() override = default;

        Ref<GfxVertFormat>    make_vert_format(const GfxVertElements& elements, const StringId& name) override;
        Ref<GfxVertBuffer>    make_vert_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxIndexBuffer>   make_index_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxUniformBuffer> make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxStorageBuffer> make_storage_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxShader>        make_shader(std::string vertex, std::string fragment, const StringId& name) override;
        Ref<GfxShader>        make_shader(Ref<Data> code, const StringId& name) override;
        Ref<GfxTexture>       make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxTexture>       make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxTexture>       make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxSampler>       make_sampler(const GfxSamplerDesc& desc, const StringId& name) override;
        Ref<GfxPipeline>      make_pipeline(const GfxPipelineState& state, const StringId& name) override;

        void update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data) override;
        void update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) override;
        void update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data) override;

        void* map_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void* map_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void* map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void* map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void begin_render_pass(const GfxRenderPassDesc& pass_desc, const StringId& name) override;
        void bind_target(const Ref<Window>& window) override;
        void bind_color_target(const Ref<GfxTexture>& texture, int target, int mip, int slice) override;
        void bind_depth_target(const Ref<GfxTexture>& texture, int mip, int slice) override;
        void viewport(const Rect2i& viewport) override;
        void clear(int target, const Vec4f& color) override;
        void clear(float depth, int stencil) override;
        bool bind_pipeline(const Ref<GfxPipeline>& pipeline) override;
        void bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset) override;
        void bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) override;
        void bind_texture(const StringId& name, int array_element, const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) override;
        void bind_texture(const GfxLocation& location, int array_element, const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) override;
        void bind_uniform_buffer(const StringId& name, int offset, int range, const Ref<GfxUniformBuffer>& buffer) override;
        void bind_uniform_buffer(const GfxLocation& location, int offset, int range, const Ref<GfxUniformBuffer>& buffer) override;
        void bind_storage_buffer(const StringId& name, int offset, int range, const Ref<GfxStorageBuffer>& buffer) override;
        void bind_storage_buffer(const GfxLocation& location, int offset, int range, const Ref<GfxStorageBuffer>& buffer) override;
        void draw(int vertex_count, int base_vertex, int instance_count) override;
        void draw_indexed(int index_count, int base_vertex, int instance_count) override;
        void end_render_pass() override;

        void shutdown() override;
        void begin_frame() override;
        void end_frame() override;
        void prepare_window(const Ref<Window>& window) override;
        void swap_buffers(const Ref<Window>& window) override;
        void flush() override;

        [[nodiscard]] const GfxDeviceCaps&   device_caps() const override;
        [[nodiscard]] const StringId&        driver_name() const override;
        [[nodiscard]] const std::string&     shader_cache_path() const override;
        [[nodiscard]] const std::string&     pipeline_cache_path() const override;
        [[nodiscard]] const std::thread::id& thread_id() const override;
        [[nodiscard]] const Mat4x4f&         clip_matrix() const override;
        [[nodiscard]] size_t                 frame_number() const override;
        [[nodiscard]] bool                   on_gfx_thread() const override;

    private:
        GfxDriverThreaded* m_driver = nullptr;
        CmdStream*         m_stream = nullptr;
        GfxDeviceCaps      m_device_caps;
        StringId           m_driver_name;
        std::thread::id    m_thread_id;
        Mat4x4f            m_clip_matrix;
        std::string        m_shader_cache_path;
        std::string        m_pipeline_cache_path;
    };

}// namespace wmoge

#endif//WMOGE_GFX_DRIVER_WRAPPER_HPP

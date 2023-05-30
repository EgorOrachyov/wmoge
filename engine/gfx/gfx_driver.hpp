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

#ifndef WMOGE_GFX_DRIVER_HPP
#define WMOGE_GFX_DRIVER_HPP

#include "core/callback_queue.hpp"
#include "core/data.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_texture.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"

#include <array>
#include <string>
#include <thread>

namespace wmoge {

    /**
     * @class GfxDriver
     * @brief Gfx driver interface
     *
     * Driver exposes gfx device object creation and rendering API.
     */
    class GfxDriver {
    public:
        virtual ~GfxDriver() = default;

        virtual Ref<GfxVertFormat>    make_vert_format(const GfxVertElements& elements, const StringId& name = StringId())                                                                                = 0;
        virtual Ref<GfxVertBuffer>    make_vert_buffer(int size, GfxMemUsage usage, const StringId& name = StringId())                                                                                    = 0;
        virtual Ref<GfxIndexBuffer>   make_index_buffer(int size, GfxMemUsage usage, const StringId& name = StringId())                                                                                   = 0;
        virtual Ref<GfxUniformBuffer> make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name = StringId())                                                                                 = 0;
        virtual Ref<GfxStorageBuffer> make_storage_buffer(int size, GfxMemUsage usage, const StringId& name = StringId())                                                                                 = 0;
        virtual Ref<GfxShader>        make_shader(std::string vertex, std::string fragment, const StringId& name = StringId())                                                                            = 0;
        virtual Ref<GfxShader>        make_shader(Ref<Data> code, const StringId& name = StringId())                                                                                                      = 0;
        virtual Ref<GfxTexture>       make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name = StringId())                   = 0;
        virtual Ref<GfxTexture>       make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name = StringId()) = 0;
        virtual Ref<GfxTexture>       make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name = StringId())                 = 0;
        virtual Ref<GfxSampler>       make_sampler(const GfxSamplerDesc& desc, const StringId& name = StringId())                                                                                         = 0;
        virtual Ref<GfxPipeline>      make_pipeline(const GfxPipelineState& state, const StringId& name = StringId())                                                                                     = 0;

        virtual void update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data)                = 0;
        virtual void update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data)              = 0;
        virtual void update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data)          = 0;
        virtual void update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data)          = 0;
        virtual void update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data)                  = 0;
        virtual void update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) = 0;
        virtual void update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data)      = 0;

        virtual void* map_vert_buffer(const Ref<GfxVertBuffer>& buffer)         = 0;
        virtual void* map_index_buffer(const Ref<GfxIndexBuffer>& buffer)       = 0;
        virtual void* map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer)   = 0;
        virtual void* map_storage_buffer(const Ref<GfxStorageBuffer>& buffer)   = 0;
        virtual void  unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer)       = 0;
        virtual void  unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer)     = 0;
        virtual void  unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) = 0;
        virtual void  unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) = 0;

        virtual void begin_render_pass(const GfxRenderPassDesc& pass_desc, const StringId& name = StringId())                                     = 0;
        virtual void bind_target(const Ref<Window>& window)                                                                                       = 0;
        virtual void bind_color_target(const Ref<GfxTexture>& texture, int target, int mip, int slice)                                            = 0;
        virtual void bind_depth_target(const Ref<GfxTexture>& texture, int mip, int slice)                                                        = 0;
        virtual void viewport(const Rect2i& viewport)                                                                                             = 0;
        virtual void clear(int target, const Vec4f& color)                                                                                        = 0;
        virtual void clear(float depth, int stencil)                                                                                              = 0;
        virtual bool bind_pipeline(const Ref<GfxPipeline>& pipeline)                                                                              = 0;
        virtual void bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset = 0)                                                = 0;
        virtual void bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset = 0)                                = 0;
        virtual void bind_texture(const StringId& name, int array_element, const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler)        = 0;
        virtual void bind_texture(const GfxLocation& location, int array_element, const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) = 0;
        virtual void bind_uniform_buffer(const StringId& name, int offset, int range, const Ref<GfxUniformBuffer>& buffer)                        = 0;
        virtual void bind_uniform_buffer(const GfxLocation& location, int offset, int range, const Ref<GfxUniformBuffer>& buffer)                 = 0;
        virtual void bind_storage_buffer(const StringId& name, int offset, int range, const Ref<GfxStorageBuffer>& buffer)                        = 0;
        virtual void bind_storage_buffer(const GfxLocation& location, int offset, int range, const Ref<GfxStorageBuffer>& buffer)                 = 0;
        virtual void draw(int vertex_count, int base_vertex, int instance_count)                                                                  = 0;
        virtual void draw_indexed(int index_count, int base_vertex, int instance_count)                                                           = 0;
        virtual void end_render_pass()                                                                                                            = 0;

        virtual void shutdown()                                = 0;
        virtual void begin_frame()                             = 0;
        virtual void end_frame()                               = 0;
        virtual void prepare_window(const Ref<Window>& window) = 0;
        virtual void swap_buffers(const Ref<Window>& window)   = 0;
        virtual void flush()                                   = 0;

        [[nodiscard]] virtual const GfxDeviceCaps&   device_caps() const         = 0;
        [[nodiscard]] virtual const StringId&        driver_name() const         = 0;
        [[nodiscard]] virtual const std::string&     shader_cache_path() const   = 0;
        [[nodiscard]] virtual const std::string&     pipeline_cache_path() const = 0;
        [[nodiscard]] virtual const std::thread::id& thread_id() const           = 0;
        [[nodiscard]] virtual const Mat4x4f&         clip_matrix() const         = 0;
        [[nodiscard]] virtual std::size_t            frame_number() const        = 0;
        [[nodiscard]] virtual bool                   on_gfx_thread() const       = 0;
    };

}// namespace wmoge

#endif//WMOGE_GFX_DRIVER_HPP

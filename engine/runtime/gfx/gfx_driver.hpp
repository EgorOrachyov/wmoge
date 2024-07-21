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

#include "core/array_view.hpp"
#include "core/async.hpp"
#include "core/callback_queue.hpp"
#include "core/data.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
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
     * Driver exposes gfx device object API (resources, limits, memory).
     */
    class GfxDriver {
    public:
        virtual ~GfxDriver() = default;

        virtual void shutdown() = 0;

        virtual Ref<GfxVertFormat>    make_vert_format(const GfxVertElements& elements, const Strid& name = Strid())                                                                                 = 0;
        virtual Ref<GfxVertBuffer>    make_vert_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                     = 0;
        virtual Ref<GfxIndexBuffer>   make_index_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                    = 0;
        virtual Ref<GfxUniformBuffer> make_uniform_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                  = 0;
        virtual Ref<GfxStorageBuffer> make_storage_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                  = 0;
        virtual Ref<GfxShader>        make_shader(GfxShaderDesc desc, const Strid& name = Strid())                                                                                                   = 0;
        virtual Ref<GfxShaderProgram> make_program(GfxShaderProgramDesc desc, const Strid& name = Strid())                                                                                           = 0;
        virtual Ref<GfxTexture>       make_texture(const GfxTextureDesc& desc, const Strid& name = Strid())                                                                                          = 0;
        virtual Ref<GfxTexture>       make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, GfxTexSwizz swizz, const Strid& name = Strid()) = 0;
        virtual Ref<GfxTexture>       make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name = Strid())  = 0;
        virtual Ref<GfxTexture>       make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name = Strid())                  = 0;
        virtual Ref<GfxSampler>       make_sampler(const GfxSamplerDesc& desc, const Strid& name = Strid())                                                                                          = 0;
        virtual Ref<GfxPsoLayout>     make_pso_layout(const GfxDescSetLayouts& layouts, const Strid& name = Strid())                                                                                 = 0;
        virtual Ref<GfxPsoGraphics>   make_pso_graphics(const GfxPsoStateGraphics& state, const Strid& name = Strid())                                                                               = 0;
        virtual Ref<GfxPsoCompute>    make_pso_compute(const GfxPsoStateCompute& state, const Strid& name = Strid())                                                                                 = 0;
        virtual Ref<GfxRenderPass>    make_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name = Strid())                                                                              = 0;
        virtual Ref<GfxRenderPass>    make_render_pass(const Ref<Window>& window, const Strid& name = Strid())                                                                                       = 0;
        virtual Ref<GfxDescSetLayout> make_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name = Strid())                                                                                = 0;
        virtual Ref<GfxDescSet>       make_desc_set(const GfxDescSetResources& resources, const Ref<GfxDescSetLayout>& layout, const Strid& name = Strid())                                          = 0;
        virtual Async                 make_shaders(const Ref<GfxAsyncShaderRequest>& request)                                                                                                        = 0;
        virtual Async                 make_psos_graphics(const Ref<GfxAsyncPsoRequestGraphics>& request)                                                                                             = 0;
        virtual Async                 make_psos_compute(const Ref<GfxAsyncPsoRequestCompute>& request)                                                                                               = 0;

        virtual void          begin_frame(std::size_t frame_id, const array_view<Ref<Window>>& windows) = 0;
        virtual GfxCmdListRef acquire_cmd_list(GfxQueueType queue_type = GfxQueueType::Graphics)        = 0;
        virtual void          submit_cmd_list(const GfxCmdListRef& cmd_list)                            = 0;
        virtual void          end_frame(bool swap_buffers = true)                                       = 0;

        [[nodiscard]] virtual const GfxDeviceCaps& device_caps() const         = 0;
        [[nodiscard]] virtual const Strid&         driver_name() const         = 0;
        [[nodiscard]] virtual const std::string&   pipeline_cache_path() const = 0;
        [[nodiscard]] virtual const Mat4x4f&       clip_matrix() const         = 0;
        [[nodiscard]] virtual std::size_t          frame_number() const        = 0;
        [[nodiscard]] virtual GfxShaderLang        shader_lang() const         = 0;
        [[nodiscard]] virtual GfxType              get_gfx_type() const        = 0;
        [[nodiscard]] virtual GfxShaderPlatform    get_shader_platform() const = 0;
        [[nodiscard]] virtual const GfxCmdListRef& get_default_list() const    = 0;
    };

}// namespace wmoge
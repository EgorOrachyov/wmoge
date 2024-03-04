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

#include "core/array_view.hpp"
#include "core/callback_queue.hpp"
#include "core/data.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_dynamic_buffers.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_pipeline_cache.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_texture.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "gfx/gfx_vert_format_cache.hpp"
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
        static constexpr const int DEFAULT_DYN_VERT_CHUNK_SIZE    = 64u * 1024u;
        static constexpr const int DEFAULT_DYN_INDEX_CHUNK_SIZE   = 64u * 1024u;
        static constexpr const int DEFAULT_DYN_UNIFORM_CHUNK_SIZE = 64u * 1024u;

        virtual ~GfxDriver() = default;

        virtual Ref<GfxVertFormat>       make_vert_format(const GfxVertElements& elements, const Strid& name = Strid())                                                                                 = 0;
        virtual Ref<GfxVertBuffer>       make_vert_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                     = 0;
        virtual Ref<GfxIndexBuffer>      make_index_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                    = 0;
        virtual Ref<GfxUniformBuffer>    make_uniform_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                  = 0;
        virtual Ref<GfxStorageBuffer>    make_storage_buffer(int size, GfxMemUsage usage, const Strid& name = Strid())                                                                                  = 0;
        virtual Ref<GfxShader>           make_shader(std::string vertex, std::string fragment, const GfxDescSetLayouts& layouts, const Strid& name = Strid())                                           = 0;
        virtual Ref<GfxShader>           make_shader(std::string compute, const GfxDescSetLayouts& layouts, const Strid& name = Strid())                                                                = 0;
        virtual Ref<GfxShader>           make_shader(Ref<Data> code, const Strid& name = Strid())                                                                                                       = 0;
        virtual Ref<GfxTexture>          make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, GfxTexSwizz swizz, const Strid& name = Strid()) = 0;
        virtual Ref<GfxTexture>          make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name = Strid())  = 0;
        virtual Ref<GfxTexture>          make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name = Strid())                  = 0;
        virtual Ref<GfxSampler>          make_sampler(const GfxSamplerDesc& desc, const Strid& name = Strid())                                                                                          = 0;
        virtual Ref<GfxPipeline>         make_pipeline(const GfxPipelineState& state, const Strid& name = Strid())                                                                                      = 0;
        virtual Ref<GfxCompPipeline>     make_comp_pipeline(const GfxCompPipelineState& state, const Strid& name = Strid())                                                                             = 0;
        virtual Ref<GfxRenderPass>       make_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name = Strid())                                                                              = 0;
        virtual Ref<GfxDynVertBuffer>    make_dyn_vert_buffer(int chunk_size, const Strid& name = Strid())                                                                                              = 0;
        virtual Ref<GfxDynIndexBuffer>   make_dyn_index_buffer(int chunk_size, const Strid& name = Strid())                                                                                             = 0;
        virtual Ref<GfxDynUniformBuffer> make_dyn_uniform_buffer(int chunk_size, const Strid& name = Strid())                                                                                           = 0;
        virtual Ref<GfxDescSetLayout>    make_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name = Strid())                                                                                = 0;
        virtual Ref<GfxDescSet>          make_desc_set(const GfxDescSetResources& resources, const Strid& name = Strid())                                                                               = 0;

        virtual void shutdown() = 0;

        virtual void begin_frame()                             = 0;
        virtual void end_frame()                               = 0;
        virtual void prepare_window(const Ref<Window>& window) = 0;
        virtual void swap_buffers(const Ref<Window>& window)   = 0;

        [[nodiscard]] virtual class GfxCtx*         ctx_immediate()  = 0;
        [[nodiscard]] virtual class GfxCtx*         ctx_async()      = 0;
        [[nodiscard]] virtual GfxPipelineCache*     pso_cache()      = 0;
        [[nodiscard]] virtual GfxCompPipelineCache* comp_pso_cache() = 0;
        [[nodiscard]] virtual GfxVertFormatCache*   vert_fmt_cache() = 0;

        [[nodiscard]] virtual GfxUniformPool*      uniform_pool()       = 0;
        [[nodiscard]] virtual GfxDynVertBuffer*    dyn_vert_buffer()    = 0;
        [[nodiscard]] virtual GfxDynIndexBuffer*   dyn_index_buffer()   = 0;
        [[nodiscard]] virtual GfxDynUniformBuffer* dyn_uniform_buffer() = 0;

        [[nodiscard]] virtual const GfxDeviceCaps&   device_caps() const         = 0;
        [[nodiscard]] virtual const Strid&           driver_name() const         = 0;
        [[nodiscard]] virtual const std::string&     shader_cache_path() const   = 0;
        [[nodiscard]] virtual const std::string&     pipeline_cache_path() const = 0;
        [[nodiscard]] virtual const std::thread::id& thread_id() const           = 0;
        [[nodiscard]] virtual const Mat4x4f&         clip_matrix() const         = 0;
        [[nodiscard]] virtual std::size_t            frame_number() const        = 0;
        [[nodiscard]] virtual bool                   on_gfx_thread() const       = 0;
        [[nodiscard]] virtual GfxShaderLang          shader_lang() const         = 0;
    };

}// namespace wmoge

#endif//WMOGE_GFX_DRIVER_HPP

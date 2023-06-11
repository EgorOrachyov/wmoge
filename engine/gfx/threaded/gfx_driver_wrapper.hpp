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

        Ref<GfxVertFormat>       make_vert_format(const GfxVertElements& elements, const StringId& name) override;
        Ref<GfxVertBuffer>       make_vert_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxIndexBuffer>      make_index_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxUniformBuffer>    make_uniform_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxStorageBuffer>    make_storage_buffer(int size, GfxMemUsage usage, const StringId& name) override;
        Ref<GfxShader>           make_shader(std::string vertex, std::string fragment, const StringId& name) override;
        Ref<GfxShader>           make_shader(Ref<Data> code, const StringId& name) override;
        Ref<GfxTexture>          make_texture_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxTexture>          make_texture_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxTexture>          make_texture_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) override;
        Ref<GfxSampler>          make_sampler(const GfxSamplerDesc& desc, const StringId& name) override;
        Ref<GfxPipeline>         make_pipeline(const GfxPipelineState& state, const StringId& name) override;
        Ref<GfxRenderPass>       make_render_pass(const GfxRenderPassDesc& pass_desc, const StringId& name) override;
        Ref<GfxDynVertBuffer>    make_dyn_vert_buffer(int chunk_size, const StringId& name) override;
        Ref<GfxDynIndexBuffer>   make_dyn_index_buffer(int chunk_size, const StringId& name) override;
        Ref<GfxDynUniformBuffer> make_dyn_uniform_buffer(int chunk_size, const StringId& name) override;

        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;
        void prepare_window(const Ref<Window>& window) override;
        void swap_buffers(const Ref<Window>& window) override;

        [[nodiscard]] class GfxCtx* ctx_immediate() override;
        [[nodiscard]] class GfxCtx* ctx_async() override;

        [[nodiscard]] GfxDynVertBuffer*    dyn_vert_buffer() override;
        [[nodiscard]] GfxDynIndexBuffer*   dyn_index_buffer() override;
        [[nodiscard]] GfxDynUniformBuffer* dyn_uniform_buffer() override;

        [[nodiscard]] const GfxDeviceCaps&   device_caps() const override;
        [[nodiscard]] const StringId&        driver_name() const override;
        [[nodiscard]] const std::string&     shader_cache_path() const override;
        [[nodiscard]] const std::string&     pipeline_cache_path() const override;
        [[nodiscard]] const std::thread::id& thread_id() const override;
        [[nodiscard]] const Mat4x4f&         clip_matrix() const override;
        [[nodiscard]] size_t                 frame_number() const override;
        [[nodiscard]] bool                   on_gfx_thread() const override;
        [[nodiscard]] GfxShaderLang          shader_lang() const override;

    private:
        GfxDriverThreaded*   m_driver             = nullptr;
        GfxDynVertBuffer*    m_dyn_vert_buffer    = nullptr;
        GfxDynIndexBuffer*   m_dyn_index_buffer   = nullptr;
        GfxDynUniformBuffer* m_dyn_uniform_buffer = nullptr;
        GfxShaderLang        m_shader_lang;
        CmdStream*           m_stream = nullptr;
        GfxDeviceCaps        m_device_caps;
        StringId             m_driver_name;
        std::thread::id      m_thread_id;
        Mat4x4f              m_clip_matrix;
        std::string          m_shader_cache_path;
        std::string          m_pipeline_cache_path;
        class GfxCtx*        m_ctx_immediate = nullptr;
        class GfxCtx*        m_ctx_async     = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_GFX_DRIVER_WRAPPER_HPP

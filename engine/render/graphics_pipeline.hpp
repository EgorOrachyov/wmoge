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

#ifndef WMOGE_GRAPHICS_PIPELINE_HPP
#define WMOGE_GRAPHICS_PIPELINE_HPP

#include "core/array_view.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_texture.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "render/render_camera.hpp"
#include "render/render_scene.hpp"
#include "render/shader_manager.hpp"
#include "render/texture_manager.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /** @brief Types of supported stages */
    enum class GraphicsPipelineStageType {
        None = 0,
        ShadowMap,
        SceneGBuffer,
        SceneForward,
        MotionBloor,
        DepthOfField,
        Bloom,
        AutoExposure,
        ToneMapping,
        SunShafts,
        Total = 10
    };

    /**
     * @class GraphicsPipelineTextures
     * @brief Pipeline textures used during rendering
    */
    struct GraphicsPipelineTextures {
        Ref<GfxTexture> depth;//< Scene geometry depth buffer
        Ref<GfxTexture> primitive_id;
        Ref<GfxTexture> velocity;
        Ref<GfxTexture> gbuffer[3];
        Ref<GfxTexture> ssao;
        Ref<GfxTexture> color_hdr;
        Ref<GfxTexture> color_ldr;

        Rect2i viewport;
        Rect2i target_viewport;

        void resize(Size2i new_target_resoulution);
        void update_viewport(Size2i new_resoulution);
    };

    /**
     * @class GraphicsPipelineStage
     * @brief
     */
    class GraphicsPipelineStage {
    public:
        GraphicsPipelineStage();
        virtual ~GraphicsPipelineStage() = default;

        void set_pipeline(class GraphicsPipeline* pipeline);

        [[nodiscard]] virtual std::string               get_name() const = 0;
        [[nodiscard]] virtual GraphicsPipelineStageType get_type() const = 0;

        [[nodiscard]] GfxDriver*              get_gfx_driver() const { return m_gfx_driver; }
        [[nodiscard]] GfxCtx*                 get_gfx_ctx() const { return m_gfx_ctx; }
        [[nodiscard]] ShaderManager*          get_shader_manager() const { return m_shader_manager; }
        [[nodiscard]] TextureManager*         get_tex_manager() const { return m_tex_manager; }
        [[nodiscard]] class RenderEngine*     get_render_engine() const { return m_render_engine; }
        [[nodiscard]] class GraphicsPipeline* get_pipeline() const { return m_pipeline; }

    protected:
        GfxDriver*              m_gfx_driver;
        GfxCtx*                 m_gfx_ctx;
        ShaderManager*          m_shader_manager;
        TextureManager*         m_tex_manager;
        class RenderEngine*     m_render_engine;
        class GraphicsPipeline* m_pipeline;
    };

    /**
     * @class GraphicsPipeline
     * @brief Pipeline capable of rendering scene cameras draw cmds into series of pass to get final image
     */
    class GraphicsPipeline {
    public:
        virtual ~GraphicsPipeline() = default;

        virtual void set_scene(RenderScene* scene);
        virtual void set_cameras(RenderCameras* cameras);
        virtual void set_views(ArrayView<struct RenderView> views);
        virtual void set_target_resolution(Size2i resolution);
        virtual void set_resolution(Size2i resolution);

        virtual void init()     = 0;
        virtual void exectute() = 0;

        virtual std::vector<GraphicsPipelineStage*> get_stages() = 0;
        virtual std::string                         get_name()   = 0;

        [[nodiscard]] const GraphicsPipelineTextures& get_textures() { return m_textures; }
        [[nodiscard]] ArrayView<struct RenderView>    get_views() const { return m_views; }
        [[nodiscard]] RenderCameras*                  get_cameras() const { return m_cameras; }
        [[nodiscard]] RenderScene*                    get_scene() const { return m_scene; }
        [[nodiscard]] const Size2i&                   get_target_resolution() const { return m_target_resolution; }
        [[nodiscard]] const Size2i&                   get_resolution() const { return m_resolution; }

    protected:
        GraphicsPipelineTextures     m_textures;
        ArrayView<struct RenderView> m_views;
        RenderCameras*               m_cameras;
        RenderScene*                 m_scene;
        Size2i                       m_target_resolution = Size2i(1280, 720);
        Size2i                       m_resolution        = Size2i(1280, 720);
    };

}// namespace wmoge

#endif//WMOGE_GRAPHICS_PIPELINE_HPP
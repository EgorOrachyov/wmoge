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
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_texture.hpp"
#include "io/yaml.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "render/render_camera.hpp"
#include "render/render_scene.hpp"
#include "render/shader_manager.hpp"
#include "render/texture_manager.hpp"
#include "resource/resource_ref.hpp"
#include "resource/texture.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class BloomSettings
     * @brief Bloom effect settings
    */
    struct BloomSettings {
        bool                             enable              = true;
        float                            intensity           = 1.0f;
        float                            threshold           = 1.0f;
        float                            knee                = 0.5f;
        float                            radius              = 4.0f;
        float                            uspample_weight     = 0.4f;
        float                            dirt_mask_intensity = 3.0f;
        std::optional<ResRef<Texture2d>> dirt_mask;

        friend Status yaml_read(const YamlConstNodeRef& node, BloomSettings& settings);
        friend Status yaml_write(YamlNodeRef node, const BloomSettings& settings);
    };

    /**
     * @class AutoExposureSettings
     * @brief Automatic exposition or eye adaptation settings
    */
    struct AutoExposureSettings {

        /** @brief Mode to select algo */
        enum class Mode {
            Adaptive = 0,
            Instant  = 1
        };

        bool  enable                = true;
        Mode  mode                  = Mode::Adaptive;
        float histogram_log_min     = -10.0f;
        float histogram_log_max     = 5.0f;
        float speed_up              = 4.0f;
        float speed_down            = 0.5f;
        float exposure_compensation = 1.0f;

        friend Status yaml_read(const YamlConstNodeRef& node, AutoExposureSettings& settings);
        friend Status yaml_write(YamlNodeRef node, const AutoExposureSettings& settings);
    };

    /**
     * @class TonemapSettings
     * @brief Final HDR image tonemapping settings for composition
    */
    struct TonemapSettings {

        /** @brief Mode to select algo */
        enum class Mode {
            Exponential      = 0,
            Reinhard         = 1,
            ReinhardExtended = 2,
            Asec             = 3,
            Uncharted2       = 4
        };

        Mode  mode        = Mode::Exponential;
        float exposure    = 1.0f;
        float white_point = 1.0f;

        friend Status yaml_read(const YamlConstNodeRef& node, TonemapSettings& settings);
        friend Status yaml_write(YamlNodeRef node, const TonemapSettings& settings);
    };

    /**
     * @class GraphicsPipelineSettings
     * @brief Graphics pipeline settings for rendering scene
    */
    struct GraphicsPipelineSettings {
        BloomSettings        bloom;
        AutoExposureSettings auto_exposure;
        TonemapSettings      tonemap;

        friend Status yaml_read(const YamlConstNodeRef& node, GraphicsPipelineSettings& settings);
        friend Status yaml_write(YamlNodeRef node, const GraphicsPipelineSettings& settings);
    };

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
        Composition,
        Total = 11
    };

    /**
     * @class GraphicsPipelineTextures
     * @brief Pipeline textures used during rendering
    */
    struct GraphicsPipelineTextures {
        Ref<GfxTexture>              depth;           //< [full] Scene geometry depth buffer
        Ref<GfxTexture>              primitive_id;    //< [full] Rendered primitive id for gbuffer effects and picking
        Ref<GfxTexture>              velocity;        //< [full] Velocity buffer
        Ref<GfxTexture>              gbuffer[3];      //< [full] GBuffer (layout see in shader)
        Ref<GfxTexture>              ssao;            //< [half] Screen space ambient occlusion
        Ref<GfxTexture>              color_hdr;       //< [full] Hdr color target for lit scene
        Ref<GfxTexture>              color_ldr;       //< [full] Ldr color target after tone mapping
        std::vector<Ref<GfxTexture>> bloom_downsample;//< [full] [half] ... Bloom downsample sample chain
        std::vector<Ref<GfxTexture>> bloom_upsample;  //< [full] [half] ... Bloom upsample sample chain

        Rect2i viewport;
        Rect2i target_viewport;
        Vec2u  size;
        Vec2u  target_size;

        void resize(Size2i new_target_resoulution);
        void update_viewport(Size2i new_resoulution);
    };

    /**
     * @class GraphicsPipelineShared
     * @brief Shared state of pipeline required for rendering
    */
    struct GraphicsPipelineShared {
        Ref<GfxStorageBuffer> lum_histogram;//< Luminance histogram of the hdr color buffer
        Ref<GfxStorageBuffer> lum_luminance;//< Luminance avg and exposure correction

        void allocate();
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
        virtual void set_settings(const GraphicsPipelineSettings& settings);

        virtual void init()     = 0;
        virtual void exectute() = 0;

        virtual std::vector<GraphicsPipelineStage*> get_stages() = 0;
        virtual std::string                         get_name()   = 0;

        [[nodiscard]] const GraphicsPipelineSettings& get_settings() { return m_settings; }
        [[nodiscard]] const GraphicsPipelineTextures& get_textures() { return m_textures; }
        [[nodiscard]] const GraphicsPipelineShared&   get_shared() { return m_shared; }
        [[nodiscard]] ArrayView<struct RenderView>    get_views() const { return m_views; }
        [[nodiscard]] RenderCameras*                  get_cameras() const { return m_cameras; }
        [[nodiscard]] RenderScene*                    get_scene() const { return m_scene; }
        [[nodiscard]] const Size2i&                   get_target_resolution() const { return m_target_resolution; }
        [[nodiscard]] const Size2i&                   get_resolution() const { return m_resolution; }

    protected:
        GraphicsPipelineSettings     m_settings;
        GraphicsPipelineTextures     m_textures;
        GraphicsPipelineShared       m_shared;
        ArrayView<struct RenderView> m_views;
        RenderCameras*               m_cameras;
        RenderScene*                 m_scene;
        Size2i                       m_target_resolution = Size2i(1280, 720);
        Size2i                       m_resolution        = Size2i(1280, 720);
    };

}// namespace wmoge

#endif//WMOGE_GRAPHICS_PIPELINE_HPP
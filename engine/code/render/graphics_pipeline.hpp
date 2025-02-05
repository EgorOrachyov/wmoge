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

#include "asset/asset_ref.hpp"
#include "core/array_view.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/texture.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "render/camera.hpp"
#include "render/render_scene.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class BloomSettings
     * @brief Bloom effect settings
    */
    struct BloomSettings {
        bool                               enable              = true;
        float                              intensity           = 1.0f;
        float                              threshold           = 1.0f;
        float                              knee                = 0.5f;
        float                              radius              = 4.0f;
        float                              uspample_weight     = 0.4f;
        float                              dirt_mask_intensity = 3.0f;
        std::optional<AssetRef<Texture2d>> dirt_mask;
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
    };

    /**
     * @class GraphicsPipelineSettings
     * @brief Graphics pipeline settings for rendering scene
    */
    struct GraphicsPipelineSettings {
        BloomSettings        bloom;
        AutoExposureSettings auto_exposure;
        TonemapSettings      tonemap;
    };

    /**
     * @class GraphicsPipelineShared
     * @brief Shared state of pipeline required for rendering
    */
    struct GraphicsPipelineAssets {
        Ref<GfxTexture> depth;       //< [full] Scene geometry depth buffer
        Ref<GfxTexture> primitive_id;//< [full] Rendered primitive id for gbuffer effects and picking
        Ref<GfxTexture> velocity;    //< [full] Velocity buffer
        Ref<GfxTexture> gbuffer[4];  //< [full] GBuffer (layout see in shader)
        Ref<GfxTexture> ssao;        //< [half] Screen space ambient occlusion
        Ref<GfxTexture> color_hdr;   //< [full] Hdr color target for lit scene
        Ref<GfxTexture> color_ldr;   //< [full] Ldr color target after tone mapping

        std::vector<Ref<GfxTexture>> bloom_downsample;//< [full] [half] ... Bloom downsample sample chain
        std::vector<Ref<GfxTexture>> bloom_upsample;  //< [full] [half] ... Bloom upsample sample chain

        Ref<GfxStorageBuffer> lum_histogram;//< Luminance histogram of the hdr color buffer
        Ref<GfxStorageBuffer> lum_luminance;//< Luminance avg and exposure correction
    };

    /**
     * @class GraphicsPipelineStage
     * @brief
    */
    class GraphicsPipelineStage {
    public:
        virtual ~GraphicsPipelineStage() = default;

        void                                  set_pipeline(class GraphicsPipeline* pipeline);
        [[nodiscard]] class GraphicsPipeline* get_pipeline() const { return m_pipeline; }

    protected:
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
        virtual void set_cameras(CameraList* cameras);
        virtual void set_views(array_view<struct RenderView> views);
        virtual void set_settings(const GraphicsPipelineSettings& settings);

        virtual void exectute() = 0;

        [[nodiscard]] array_view<GraphicsPipelineStage*> get_stages() { return m_stages; }
        [[nodiscard]] const GraphicsPipelineSettings&    get_settings() { return m_settings; }
        [[nodiscard]] const GraphicsPipelineAssets&      get_assets() { return m_assets; }
        [[nodiscard]] array_view<struct RenderView>      get_views() const { return m_views; }
        [[nodiscard]] CameraList*                        get_cameras() const { return m_cameras; }
        [[nodiscard]] RenderScene*                       get_scene() const { return m_scene; }

    protected:
        std::vector<GraphicsPipelineStage*> m_stages;
        GraphicsPipelineSettings            m_settings;
        GraphicsPipelineAssets              m_assets;
        array_view<struct RenderView>       m_views;
        CameraList*                         m_cameras;
        RenderScene*                        m_scene;
    };

}// namespace wmoge
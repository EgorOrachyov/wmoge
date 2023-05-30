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

#ifndef WMOGE_RENDER_PIPELINE_HPP
#define WMOGE_RENDER_PIPELINE_HPP

#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "math/color.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"
#include "render/render_scene.hpp"
#include "render/render_view.hpp"

#include <array>
#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class RenderPipelineStage
     * @brief Single stage in a graphic pipeline
     */
    class RenderPipelineStage {
    public:
        virtual ~RenderPipelineStage() = default;

        virtual const StringId& get_name() const           = 0;
        virtual void            on_register()              = 0;
        virtual void            on_execute(int view_index) = 0;

        class RenderPipeline* get_pipeline();
        class RenderScene*    get_render_scene();
        class GfxDriver*      get_gfx_driver();
        RenderView*           get_view(int view_index);

        void set_pipeline(class RenderPipeline* pipeline) { m_pipeline = pipeline; }

    private:
        class RenderPipeline* m_pipeline = nullptr;
    };

    /**
     * @class RenderPipeline
     * @brief An interface for a pipeline responsible for a scene views rendering
     */
    class RenderPipeline {
    public:
        RenderPipeline();
        virtual ~RenderPipeline() = default;

        virtual void execute()         = 0;
        virtual void allocate_views()  = 0;
        virtual void collect_objects() = 0;
        virtual void sort_queues()     = 0;
        virtual void render()          = 0;

        void set_scene(RenderScene* render_scene);

        const fast_vector<Ref<RenderView>>&                        get_views();
        const fast_vector<RenderPipelineStage*>&                   get_stages();
        const std::array<RenderPipelineStage*, int(DrawPass::Max)> get_passes();
        RenderView*                                                get_view(int view_index);
        RenderPipelineStage*                                       get_pass(DrawPass pass);
        RenderScene*                                               get_render_scene();
        GfxDriver*                                                 get_gfx_driver();

    protected:
        /** list of views allocated each frame to rendered */
        fast_vector<Ref<RenderView>> m_views;
        /** list with all stages for rendering */
        fast_vector<RenderPipelineStage*> m_stages;
        /** list with stages processing specific passes */
        std::array<RenderPipelineStage*, int(DrawPass::Max)> m_passes;
        /** scene to render (1:1 mapping for now) */
        RenderScene* m_render_scene = nullptr;
        /** cache driver ptr to exec commands */
        GfxDriver* m_gfx_driver = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_PIPELINE_HPP

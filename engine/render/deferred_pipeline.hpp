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

#ifndef WMOGE_DEFERRED_PIPELINE_HPP
#define WMOGE_DEFERRED_PIPELINE_HPP

#include "gfx/gfx_texture.hpp"
#include "render/graphics_pipeline.hpp"

namespace wmoge {

    /**
     * @class DeferredPipeline
     * @brief Deferred HDR pipeline for scene rendering
    */
    class DeferredPipeline : public GraphicsPipeline {
    public:
        DeferredPipeline();
        DeferredPipeline(const DeferredPipeline&) = delete;
        DeferredPipeline(DeferredPipeline&&)      = delete;
        ~DeferredPipeline();

        void init() override;
        void exectute() override;

        std::vector<GraphicsPipelineStage*> get_stages() override;
        std::string                         get_name() override;

    private:
        std::unique_ptr<class PassGBuffer> m_pass_gbuffer;
        std::unique_ptr<class PassToneMap> m_pass_tonemap;
        // std::unique_ptr<class PassForward>          m_pass_forward;
        // std::unique_ptr<class PassDeferredLighting> m_pass_deferred_lighting;
        // std::unique_ptr<class PassBloop>            m_pass_bloom;

        std::vector<GraphicsPipelineStage*> m_stages;
    };

}// namespace wmoge

#endif//WMOGE_DEFERRED_PIPELINE_HPP
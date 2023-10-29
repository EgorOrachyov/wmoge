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

#ifndef WMOGE_PASS_BLOOM_HPP
#define WMOGE_PASS_BLOOM_HPP

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_sampler.hpp"
#include "render/graphics_pipeline.hpp"
#include "render/render_engine.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class PassBloom
     * @brief Executes bloom pass of hdr color target
    */
    class PassBloom : public GraphicsPipelineStage {
    public:
        PassBloom();

        void execute(int view_idx);

        std::string               get_name() const override;
        GraphicsPipelineStageType get_type() const override;

    private:
        Ref<GfxPipeline> m_pipeline_downsample_prefilter;
        Ref<GfxPipeline> m_pipeline_downsample;
        Ref<GfxPipeline> m_pipeline_upsample;
        Ref<GfxSampler>  m_sampler;
    };

}// namespace wmoge

#endif//WMOGE_PASS_BLOOM_HPP
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
     * @class GraphicsPipelineStage
     * @brief
     */
    class GraphicsPipelineStage {
    public:
        virtual ~GraphicsPipelineStage() = default;

        [[nodiscard]] virtual std::string               get_name() const = 0;
        [[nodiscard]] virtual GraphicsPipelineStageType get_type() const = 0;
    };

    /**
     * @class GraphicsPipeline
     * @brief
     */
    class GraphicsPipeline {
    public:
        virtual ~GraphicsPipeline() = default;
    };

}// namespace wmoge

#endif//WMOGE_GRAPHICS_PIPELINE_HPP
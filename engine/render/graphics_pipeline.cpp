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

#include "graphics_pipeline.hpp"

#include "debug/profiler.hpp"

#include <cstring>

namespace wmoge {

    WG_IO_BEGIN(BloomSettings)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(enable)
    WG_IO_FIELD_OPT(intensity)
    WG_IO_FIELD_OPT(threshold)
    WG_IO_FIELD_OPT(knee)
    WG_IO_FIELD_OPT(radius)
    WG_IO_FIELD_OPT(uspample_weight)
    WG_IO_FIELD_OPT(dirt_mask_intensity)
    WG_IO_FIELD_OPT(dirt_mask)
    WG_IO_END(BloomSettings)

    WG_IO_BEGIN(AutoExposureSettings)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(enable)
    WG_IO_FIELD_OPT(mode)
    WG_IO_FIELD_OPT(histogram_log_min)
    WG_IO_FIELD_OPT(histogram_log_max)
    WG_IO_FIELD_OPT(speed_up)
    WG_IO_FIELD_OPT(speed_down)
    WG_IO_FIELD_OPT(exposure_compensation)
    WG_IO_END(AutoExposureSettings)

    WG_IO_BEGIN(TonemapSettings)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(mode)
    WG_IO_FIELD_OPT(exposure)
    WG_IO_FIELD_OPT(white_point)
    WG_IO_END(TonemapSettings)

    WG_IO_BEGIN(GraphicsPipelineSettings)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(bloom)
    WG_IO_FIELD_OPT(auto_exposure)
    WG_IO_FIELD_OPT(tonemap)
    WG_IO_END(GraphicsPipelineSettings)

    void GraphicsPipeline::set_scene(RenderScene* scene) {
        m_scene = scene;
    }
    void GraphicsPipeline::set_cameras(CameraList* cameras) {
        m_cameras = cameras;
    }
    void GraphicsPipeline::set_views(ArrayView<struct RenderView> views) {
        m_views = views;
    }
    void GraphicsPipeline::set_settings(const GraphicsPipelineSettings& settings) {
        m_settings = settings;
    }

}// namespace wmoge

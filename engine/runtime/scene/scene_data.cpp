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

#include "scene_data.hpp"

#include "profiler/profiler.hpp"

namespace wmoge {

    WG_IO_BEGIN(SceneDataSpatial)
    WG_IO_END(SceneDataSpatial)

    WG_IO_BEGIN(SceneDataCamera)
    WG_IO_FIELD_OPT(name)
    WG_IO_FIELD_OPT(color)
    WG_IO_FIELD_OPT(fov)
    WG_IO_FIELD_OPT(near)
    WG_IO_FIELD_OPT(far)
    WG_IO_FIELD_OPT(projection)
    WG_IO_END(SceneDataCamera)

    WG_IO_BEGIN(SceneData)
    WG_IO_PROFILE()
    WG_IO_FIELD(name)
    WG_IO_FIELD(entities)
    WG_IO_FIELD(names)
    WG_IO_FIELD(hier)
    WG_IO_FIELD(cameras)
    WG_IO_FIELD(pipeline)
    WG_IO_END(SceneData)

    void SceneDataCamera::fill(EcsComponentCamera& component) const {
        Camera& camera = component.camera;
        camera.set_fov(fov);
        camera.set_near_far(near, far);
        camera.set_color(color);
        camera.set_proj(projection);
        camera.set_name(name);
    }

}// namespace wmoge
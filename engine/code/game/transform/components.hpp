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

#include "ecs/ecs_component.hpp"
#include "ecs/ecs_entity.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"

#include <vector>

namespace wmoge {

    struct GmParentComponent : public EcsComponent<GmParentComponent> {
        EcsEntity id;
    };

    struct GmChildrenComponent : public EcsComponent<GmChildrenComponent> {
        std::vector<EcsEntity> ids;
    };

    struct GmTransformComponent : public EcsComponent<GmTransformComponent> {
        Transform3d t;
    };

    struct GmTransformFrameComponent : public EcsComponent<GmTransformFrameComponent> {
        int  frame    = -1;
        bool is_dirty = true;
    };

    struct GmMatLocalToWorldComponent : public EcsComponent<GmMatLocalToWorldComponent> {
        Mat3x4f m = Math3d::identity_m3x4f();
    };

    struct GmMatLocalToWorldPrevComponent : public EcsComponent<GmMatLocalToWorldPrevComponent> {
        Mat3x4f m = Math3d::identity_m3x4f();
    };

    struct GmMatWorldToLocalComponent : public EcsComponent<GmMatWorldToLocalComponent> {
        Mat3x4f m = Math3d::identity_m3x4f();
    };

    struct GmMatLocalComponent : public EcsComponent<GmMatLocalComponent> {
        Mat3x4f m = Math3d::identity_m3x4f();
    };

}// namespace wmoge
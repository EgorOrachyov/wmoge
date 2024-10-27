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
#include "material/material.hpp"
#include "math/mat.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "mesh/mesh.hpp"

#include <string>

namespace wmoge {

    enum class GmDebugPrimitiveType {
        Sphere,
        Cylinder,
        Cone,
        Box
    };

    struct GmDebugDistMinMaxComponent : public EcsComponent<GmDebugDistMinMaxComponent> {
        int dist_min;
        int dist_max;
    };

    struct GmDebugMeshComponent : public EcsComponent<GmDebugMeshComponent> {
        Ref<Mesh>     mesh;
        Ref<Material> material;
    };

    struct GmDebugLabelComponent : public EcsComponent<GmDebugLabelComponent> {
        std::string text;
        Color4f     color;
        float       size;
    };

    struct GmDebugPrimitiveComponent : public EcsComponent<GmDebugPrimitiveComponent> {
        GmDebugPrimitiveType type;
        Color4f              color;
    };

}// namespace wmoge
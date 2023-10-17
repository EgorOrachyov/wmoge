/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"

RenderObjectData GetRenderObjectDataDefault() {
    RenderObjectData data;
    data.LocalToWorld = GetIdentity4x4();
    data.LocalToWorldPrev = GetIdentity4x4();
    data.WorldToLocal = GetIdentity4x4();
    data.WorldToLocalPrev = GetIdentity4x4();
    data.NormalMatrix = GetIdentity4x4();
    data.AabbPos = vec4(0,0,0,0);
    data.AabbSizeHalf = vec4(0,0,0,0);

    return data;
}

RenderObjectData GetRenderObjectDataById(in int primitiveId) {
    return RenderObjects[primitiveId];
}

vec3 TransformLocalToWorld(in vec3 posLocal, in int primitiveId) {
    return TransformLocalToWorld(posLocal, GetRenderObjectDataById(primitiveId).LocalToWorld);
}

vec3 TransformLocalToWorldNormal(in vec3 normLocal, in int primitiveId) {
    return TransformLocalToWorldNormal(normLocal, GetRenderObjectDataById(primitiveId).NormalMatrix);
}
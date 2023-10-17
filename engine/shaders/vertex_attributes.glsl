/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

struct VertexAttributes {
    vec3 pos3;
    vec2 pos2;
    vec3 norm;
    vec3 tang;
    ivec4 boneIds;
    vec4 boneWeigths;
    vec4 col[4];
    vec2 uv[4];
    int primitiveId;
};

VertexAttributes GetVertexAttributesDefault() {
    VertexAttributes attributes;
    attributes.pos3 = vec3(0,0,0);
    attributes.pos2 = vec2(0,0);
    attributes.norm = vec3(0,0,0);
    attributes.tang = vec3(0,0,0);
    attributes.boneIds = ivec4(0,0,0,0);
    attributes.boneWeigths = vec4(0,0,0,0);
    attributes.col[0] = vec4(0,0,0,0);
    attributes.col[1] = vec4(0,0,0,0);
    attributes.col[2] = vec4(0,0,0,0);
    attributes.col[3] = vec4(0,0,0,0);
    attributes.uv[0] = vec2(0,0);
    attributes.uv[1] = vec2(0,0);
    attributes.uv[2] = vec2(0,0);
    attributes.uv[3] = vec2(0,0);
    attributes.primitiveId = -1;
    
    return attributes;
}

VertexAttributes ReadVertexAttributes() {
    VertexAttributes attributes = GetVertexAttributesDefault();

    #ifdef ATTRIB_Pos3f
        attributes.pos3 = inPos3f;
    #endif

    #ifdef ATTRIB_Pos2f
        attributes.pos2 = inPos2f;
    #endif

    #ifdef ATTRIB_Norm3f
        attributes.norm = inNorm3f;
    #endif

    #ifdef ATTRIB_Tang3f
        attributes.tang = inTang3f;
    #endif

    #ifdef ATTRIB_BoneIds4i
        attributes.boneIds = inBoneIds4i;
    #endif

    #ifdef ATTRIB_BoneWeights4f
        attributes.boneWeigths = inBoneWeights4f;
    #endif

    #ifdef ATTRIB_Col04f
        attributes.col[0] = inCol04f;
    #endif

    #ifdef ATTRIB_Col14f
        attributes.col[1] = inCol14f;
    #endif

    #ifdef ATTRIB_Col24f
        attributes.col[2] = inCol24f;
    #endif

    #ifdef ATTRIB_Col34f
        attributes.col[3] = inCol34f;
    #endif

    #ifdef ATTRIB_Uv02f
        attributes.uv[0] = inUv02f;
    #endif 

    #ifdef ATTRIB_Uv12f
        attributes.uv[1] = inUv12f;
    #endif 

    #ifdef ATTRIB_Uv22f
        attributes.uv[2] = inUv22f;
    #endif 

    #ifdef ATTRIB_Uv32f
        attributes.uv[3] = inUv32f;
    #endif

    #ifdef ATTRIB_PrimitiveIdi
        attributes.primitiveId = inPrimitiveIdi;
    #endif

    return attributes;
}
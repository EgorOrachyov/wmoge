/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

struct VertexAttributes {
    vec3  pos3;
    vec2  pos2;
    vec3  norm;
    vec3  tang;
    ivec4 boneIds;
    vec4  boneWeigths;
    vec4  col[4];
    vec2  uv[4];
    int   renderObjectId;
    int   renderInstanceId;
};

VertexAttributes GetVertexAttributesDefault() {
    VertexAttributes attributes;
    attributes.pos3        = vec3(0, 0, 0);
    attributes.pos2        = vec2(0, 0);
    attributes.norm        = vec3(0, 0, 0);
    attributes.tang        = vec3(0, 0, 0);
    attributes.boneIds     = ivec4(0, 0, 0, 0);
    attributes.boneWeigths = vec4(0, 0, 0, 0);
    attributes.col[0]      = vec4(0, 0, 0, 0);
    attributes.col[1]      = vec4(0, 0, 0, 0);
    attributes.col[2]      = vec4(0, 0, 0, 0);
    attributes.col[3]      = vec4(0, 0, 0, 0);
    attributes.uv[0]       = vec2(0, 0);
    attributes.uv[1]       = vec2(0, 0);
    attributes.uv[2]       = vec2(0, 0);
    attributes.uv[3]       = vec2(0, 0);
    attributes.objectId    = -1;
    attributes.instanceId  = -1;

    return attributes;
}

VertexAttributes ReadVertexAttributes() {
    VertexAttributes attributes = GetVertexAttributesDefault();

    #ifdef ATTRIB_POS_3F
        attributes.pos3 = inPos3f;
    #endif     

    #ifdef ATTRIB_POS_2F
        attributes.pos2 = inPos2f;
    #endif     

    #ifdef ATTRIB_NORM_3F
        attributes.norm = inNorm3f;
    #endif     

    #ifdef ATTRIB_TANG_3F
        attributes.tang = inTang3f;
    #endif     

    #ifdef ATTRIB_BONE_IDS_4I
        attributes.boneIds = inBoneIds4i;
    #endif     

    #ifdef ATTRIB_BONE_WEIGHTS_4F
        attributes.boneWeigths = inBoneWeights4f;
    #endif     

    #ifdef ATTRIB_COL0_4F
        attributes.col[0] = inCol04f;
    #endif     

    #ifdef ATTRIB_COL1_4F
        attributes.col[1] = inCol14f;
    #endif     

    #ifdef ATTRIB_COL2_4F
        attributes.col[2] = inCol24f;
    #endif     

    #ifdef ATTRIB_COL3_4F
        attributes.col[3] = inCol34f;
    #endif     

    #ifdef ATTRIB_UV0_2F
        attributes.uv[0] = inUv02f;
    #endif     

    #ifdef ATTRIB_UV1_2F
        attributes.uv[1] = inUv12f;
    #endif     

    #ifdef ATTRIB_UV2_2F
        attributes.uv[2] = inUv22f;
    #endif     

    #ifdef ATTRIB_UV3_2F
        attributes.uv[3] = inUv32f;
    #endif     

    #ifdef ATTRIB_OBJECT_ID
        attributes.objectId = inObjectId;
    #endif     

    #ifdef ATTRIB_INSTANCE_ID
        attributes.instanceId = inInstanceId;
    #endif

    return attributes;
}
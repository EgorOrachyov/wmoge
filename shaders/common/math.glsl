#include "common/defines.glsl"

// Flip optionally UV (for GL or VK to work uniformly)
vec2 UnpackUv(in vec2 uv) {
    #ifdef TARGET_VULKAN
        return vec2(uv.x, 1.0f - uv.y);
    #else
        return uv;
    #endif
}

// Construct mat3 identity matrix
mat3 GetIdentity3x3() {
    mat4 matrix = mat3(
        vec3(1,0,0),
        vec3(0,1,0),
        vec3(0,0,1));
        
    return matrix;
}

// Construct mat4 identity matrix
mat4 GetIdentity4x4() {
    mat4 matrix = mat4(
        vec4(1,0,0,0),
        vec4(0,1,0,0),
        vec4(0,0,1,0),
        vec4(0,0,0,1));
        
    return matrix;
}

vec3 TransformLocalToWorld(in vec3 posLocal, in mat4 localToWorld) {
    return (localToWorld * vec4(posLocal, 1.0f)).xyz;
}

vec3 TransformLocalToWorldNormal(in vec3 normLocal, in mat4 normalMatrix) {
    return (normalMatrix * vec4(normLocal, 0.0f)).xyz;
}

vec2 GidToUv(uvec2 gid, ivec2 size) {
    return (vec2(gid) + vec2(0.5f, 0.5f)) / vec2(size);
}

float Saturate(float x) {
    return clamp(x, 0.0f, 1.0f);
}

uint Flatten3D(uvec3 coord, uvec3 dim) {
    return coord.x * dim.y * dim.z + coord.y * dim.z + coord.z;
}

uvec3 Unflatten3D(uint index, uvec3 dim) {
    uvec3 coord;

    coord.x = index / (dim.y * dim.z);
    index -= coord.x * dim.y * dim.z;

    coord.y = index / (dim.z);
    index -= coord.y * dim.z;

    coord.z = index;

    return coord;
}

// Linearize depth from [0,1] to linear depth value in [nearZ, farZ]
float DepthToZ(in float depth, in float nearZ, in float farZ) {
    const float pz = depth * 2.0f - 1.0f;
    return (2.0 * nearZ * farZ) / (farZ + nearZ - pz * (farZ - nearZ)); 
}

// Project 3d view space point to Ndc coordinates
vec3 ProjectPoint(in vec3 p, in mat4 projection) {
    vec4 projected = projection * vec4(p, 1.0f);
    projected.xyz /= projected.w;
    return projected.xyz;
}

// Reconstruct Ndc point to 3d view space point 
vec3 NdcToView(in vec3 ndc, in mat4 projInv) {
    return ProjectPoint(ndc, projInv);
}

// Convert screen space [0, 1] uv and [0, 1] depth to Ndc point
vec3 UvDepthToNdc(in vec2 uv, in float depth) {
    return vec3(uv, depth) * 2.0f - 1.0f;
}
#include "generated/declarations.glsl"

#include "common/defines.glsl"
#include "common/math.glsl"

#define GROUP_SIZE_DEFAULT 8

layout(local_size_x = GROUP_SIZE_DEFAULT, local_size_y = GROUP_SIZE_DEFAULT, local_size_z = 1) in;

void main()
{
    const uvec2 gid = gl_GlobalInvocationID.xy;
    const ivec2 size = imageSize(Result);

    if (gid.x >= size.x || gid.y >= size.y)
    {
        return;
    }

    imageStore(Result, ivec2(gid), FillValue);
}
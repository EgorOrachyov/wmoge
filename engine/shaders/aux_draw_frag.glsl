#include "generated/declarations.glsl"

#include "common/math.glsl"
#include "common/color.glsl"

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 vsCol;
layout(location = 1) in vec2 vsUv;

void main()
{
    vec4 color = texture(ImageTexture, vsUv).rgba * vsCol;

#ifdef OUT_MODE_SRGB
    color.rgb = ColorSrgbToLinear(color.rgb, InverseGamma);
#endif

    outColor = color;
}
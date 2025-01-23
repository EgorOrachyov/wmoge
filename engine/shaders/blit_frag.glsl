#include "generated/declarations.glsl"

#include "common/math.glsl"
#include "common/color.glsl"

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 vsUv;

void main()
{
    vec3 color = texture(ImageTexture, vsUv).rgb;

#ifdef OUT_MODE_SRGB
    color.rgb = ColorSrgbToLinear(color.rgb, InverseGamma);
#endif

    outColor = vec4(color, 1.0f);
}
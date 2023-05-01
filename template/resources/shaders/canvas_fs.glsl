layout (location = 0) out vec4 fs_color;

layout (location = 0) in vec4 in_color;
layout (location = 1) in vec2 in_uv;

layout (set = 0, binding = 1) uniform sampler2D Texture;

vec3 srgb_to_linear(vec3 color) {
    const float gamma = 2.2f;
    return pow(color, vec3(gamma));
}

void main() {
    #ifdef CANVAS_FONT_BITMAP
    float mask = texture(Texture, in_uv).r;
    fs_color = vec4(in_color.rgb, in_color.a * mask);
    #else
    vec4 tex_color_srgb = texture(Texture, in_uv).rgba;
    vec4 tex_color_linear = vec4(srgb_to_linear(tex_color_srgb.rgb), tex_color_srgb.a);
    fs_color = vec4(in_color.rgba * tex_color_linear);
    #endif
}
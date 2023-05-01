layout (location = 0) out vec4 fs_color;

layout (location = 0) in vec3 in_color;
layout (location = 1) in vec2 in_uv;

layout (set = 0, binding = 1) uniform sampler2D FontBitmap;

void main() {
    fs_color = vec4(in_color.rgb, texture(FontBitmap, in_uv).r);
}
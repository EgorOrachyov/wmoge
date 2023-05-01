layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_col;

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec2 out_uv;

layout (set = 0, binding = 0, std140) uniform Constants {
    mat4 clip_proj_screen;
};

void main() {
    out_color = in_col;
    out_uv = vec2(in_uv.x, 1.0f - in_uv.y);
    gl_Position = clip_proj_screen * vec4(in_pos, 0.0f, 1.0f);
}
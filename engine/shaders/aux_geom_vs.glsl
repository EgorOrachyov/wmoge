layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_col;

layout (location = 0) out vec3 out_color;

layout (set = 0, binding = 0, std140) uniform Constants {
    mat4 clip_proj_view;
    mat4 clip_proj_screen;
};

void main() {
    out_color = in_col;
    gl_Position = clip_proj_view * vec4(in_pos, 1.0f);
}
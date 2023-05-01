layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;

void main() {
    ShaderParamsVs params;

    params.pos = in_pos;
    params.uv = vec2(in_uv.x, 1.0f - in_uv.y);
    params.color = in_color;

    vertex(params);

    fs_uv = params.uv;
    fs_color = params.color;
    gl_Position = clip_proj_view * params.world_pos;
}
layout(location = 0) out vec4 out_color;

#define GAMMA_CORRECTION

vec4 srgb_to_linear(vec4 color) {
    const float gamma = 2.2f;
    return vec4(pow(color.rgb, vec3(gamma)), color.a);
}

void main() {
    ShaderParamsFs params;

    params.uv           = fs_uv;
    params.color        = fs_color;
    params.result_color = fs_color;

    fragment(params);

#ifdef GAMMA_CORRECTION
    out_color = srgb_to_linear(params.result_color);
#endif
}
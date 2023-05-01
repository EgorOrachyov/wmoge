layout (location = 0) out vec4 fs_color;

layout (location = 0) in vec3 in_color;

void main() {
    fs_color = vec4(in_color, 1.0f);
}
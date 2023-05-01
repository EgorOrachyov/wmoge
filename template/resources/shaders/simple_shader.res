# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: MaterialShader
deps: [ ]
description: "simple material shader which can be customized"
params:
  keywords: [ ]
  domain: default
  render_queue: default
  poly_mode: Fill
  cull_mode: Disabled
  front_face: CounterClockwise
  depth_enable: false
  depth_write: true
  depth_func: Less
  parameters:
    - name: "color"
      type: Vec3
      value: "0.2 0.3 0.4"
    - name: "time"
      type: Float
      value: "0.0"
  textures:
    - name: "sprite"
      type: Tex2d
      value: "white"
  vertex: >
    layout (location = 0) out vec2 frag_uv0;

    void main() {
        frag_uv0 = wg_in_uv0;
        gl_Position = vec4(wg_in_position.x * sin(time), wg_in_position.y * cos(time), 0.0f, 1.0f);
    }
  fragment: >
    layout (location = 0) in vec2 frag_uv0;

    void main() {
        wg_out_color = vec4(texture(sprite, frag_uv0).rgb * color, 1.0f);
    }

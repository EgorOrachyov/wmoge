# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: Shader2d
deps: [ ]
description: ""
params:
  keywords: [ ]
  domain: default
  render_queue: default
  poly_mode: Fill
  cull_mode: Disabled
  front_face: CounterClockwise
  depth_enable: false
  depth_write: false
  depth_func: Less
  parameters:
    - name: "dummy"
      type: Vec2
      value: "0.0 0.0"
  textures:
    - name: "font"
      type: Tex2d
      value: "res://textures/default/red"
  vertex: >
    void vertex(inout ShaderParamsVs params) {
        // apply standard transform
        params.world_pos = model * vec4(params.pos + dummy, 0.0f, 1.0f);
    }
  fragment: >
    void fragment(inout ShaderParamsFs params) {
        // final color combined color
        params.result_color = vec4(1, 1, 1, texture(font, params.uv).r) * params.color * tint;
    }

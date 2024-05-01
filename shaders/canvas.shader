name: canvas
domain: Engine
extends: ""
ui_name: ""
ui_hint: "shader to draw 2d primitives onto painting canvas"
param_blocks:
  - name: default
    params:
      - { name: ClipProjView, type: mat4, ui_name: "", ui_hint: "" }
      - { name: InverseGamma, type: float, ui_name: "", ui_hint: "" }
      - { name: DrawCmdsBuffer, type: DrawCmdsBuffer, binding: StorageBuffer, ui_name: "", ui_hint: "" }
  - name: images-batch
    params:
      - { name: Image0, type: sampler2D, value: White, ui_name: "", ui_hint: ""}
      - { name: Image1, type: sampler2D, value: White, ui_name: "", ui_hint: ""}
      - { name: Image2, type: sampler2D, value: White, ui_name: "", ui_hint: ""}
      - { name: Image3, type: sampler2D, value: White, ui_name: "", ui_hint: ""}
techniques:
  - name: default
    tags: []
    options:
      - { name: OUT_MODE, variants: [ SRGB, LINEAR ] }
    passes:
      - name: default
        tags: []
        state:
          cull_mode: Disabled
          depth_enable: false
          depth_write: false
          blending: true
sources:
  - { module: Vertex, file: "canvas_vert.glsl" }
  - { module: Fragment, file: "canvas_frag.glsl" } 
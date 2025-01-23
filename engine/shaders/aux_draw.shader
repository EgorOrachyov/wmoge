name: aux_draw
domain: Graphics
ui_hint: "shader to draw aux draw primitives in scene"
sources:
  - { module: Vertex, file: "engine/shaders/aux_draw_vert.glsl" }
  - { module: Fragment, file: "engine/shaders/aux_draw_frag.glsl" } 
param_blocks:
  - name: default
    params:
      - { name: ClipProjView, type: mat4 }
      - { name: InverseGamma, type: float }
      - { name: ImageTexture, type: sampler2D, value: White }
techniques:
  - name: default
    tags: []
    options:
      - { name: OUT_MODE, variants: [ SRGB, LINEAR ] }
    passes:
      - name: solid
        tags: []
        state:
          prim_type: Triangles
          rs:
            poly_mode: Fill
            cull_mode: Disabled
          ds:
            depth_enable: false
            depth_write: false
          bs:
            blending: true
      - name: wire
        tags: []
        state:
          prim_type: Lines
          rs:
            poly_mode: Fill
            cull_mode: Disabled
          ds:
            depth_enable: false
            depth_write: false
          bs:
            blending: false
      - name: text
        tags: []
        state:
          prim_type: Triangles
          rs:
            poly_mode: Fill
            cull_mode: Disabled
          ds:
            depth_enable: false
            depth_write: false
          bs:
            blending: true
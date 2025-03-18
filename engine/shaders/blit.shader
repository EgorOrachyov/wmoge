name: blit
domain: Graphics
ui_hint: "shader to blit texture content"
sources:
  - { module: Vertex, file: "engine/shaders/blit_vert.glsl" }
  - { module: Fragment, file: "engine/shaders/blit_frag.glsl" } 
param_blocks:
  - name: default
    params:
      - { name: Gamma, type: float }
      - { name: InverseGamma, type: float }
      - { name: ImageTexture, type: sampler2D, value: White }
techniques:
  - name: default
    tags: []
    options:
      - { name: OUT_MODE, variants: [ NONE, SRGB, LINEAR ] }
    passes:
      - name: default
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
            blending: false
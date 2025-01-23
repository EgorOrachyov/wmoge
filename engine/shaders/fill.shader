name: fill
domain: Compute
ui_hint: "shader to fill an image"
sources:
  - { module: Compute, file: "engine/shaders/fill_comp.glsl" }
constants:
  - { name: GROUP_SIZE_DEFAULT, type: int, value: 8 }
param_blocks:
  - name: default
    params:
      - { name: FillValue, type: vec4 }
      - { name: Result, type: image2D, qualifiers: [Rgba16f, Writeonly] }
techniques:
  - name: default
    tags: []
    passes:
      - name: default
        tags: []
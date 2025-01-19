name: canvas
domain: Graphics
ui_hint: "shader to draw 2d primitives onto painting canvas"
sources:
  - { module: Vertex, file: "engine/shaders/canvas_vert.glsl" }
  - { module: Fragment, file: "engine/shaders/canvas_frag.glsl" } 
constants:
  - { name: MAX_IMAGES, type: int, value: 4 }
structs:
  - name: GpuCanvasDrawCmdData
    fields:
      - { name: Transform0, type: vec4 }
      - { name: Transform1, type: vec4 }
      - { name: Transform2, type: vec4 }
      - { name: ClipRect, type: vec4 }
      - { name: TextureIdx, type: int }
      - { name: pad0, type: int }
      - { name: pad1, type: int }
      - { name: pad2, type: int }
  - name: GpuCanvasDrawCmdsBuffer
    fields: 
      - { name: DrawCmds, type: GpuCanvasDrawCmdData, array_size: -1 }
param_blocks:
  - name: default
    params:
      - { name: ClipProjView, type: mat4, ui_name: "", ui_hint: "" }
      - { name: InverseGamma, type: float, ui_name: "", ui_hint: "" }
      - { name: GpuCanvasDrawCmdsBuffer, type: GpuCanvasDrawCmdsBuffer, binding: StorageBuffer, ui_name: "", ui_hint: "" }
  - name: images_batch
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
          rs:
            cull_mode: Disabled
          ds:
            depth_enable: false
            depth_write: false
          bs:
            blending: true
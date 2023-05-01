# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: Material
deps: [
  "res://shaders/simple_shader",
  "res://textures/def_pattern"
]
description: "simple material for test"
params:
  shader: "res://shaders/simple_shader"
  parameters:
    - name: "color"
      value: "0.4 0.3 0.2"
    - name: "time"
      value: "0.1"
  textures:
    - name: "sprite"
      value: "res://textures/def_pattern"
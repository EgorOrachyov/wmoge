# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: Sprite
deps: [ "res://textures/column" ]
description: ""
params:
  size_x: 10
  size_y: 40
  pivot_x: 5
  pivot_y: 40
  animations:
    - name: idle
      texture: "res://textures/column"
      speed: 0.0
      loop: false
      frames:
        - 0.0 0.0 1.0 1.0
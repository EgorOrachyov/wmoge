# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: Sprite
deps: [ "res://textures/background_1280x720" ]
description: "background"
params:
  size_x: 1280
  size_y: 720
  pivot_x: 0
  pivot_y: 0
  animations:
    - name: idle
      texture: "res://textures/background_1280x720"
      speed: 0.0
      loop: false
      frames:
        - 0.0 0.0 1.0 1.0
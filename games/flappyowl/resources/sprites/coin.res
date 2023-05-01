# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: Sprite
deps: [ "res://textures/coin" ]
description: ""
params:
  size_x: 6
  size_y: 6
  pivot_x: 3
  pivot_y: 3
  animations:
    - name: idle
      texture: "res://textures/coin"
      speed: 6.0
      loop: true
      frames:
        - 0.0 0.0 0.25 1.0
        - 0.25 0.0 0.25 1.0
        - 0.5 0.0 0.25 1.0
        - 0.75 0.0 0.25 1.0
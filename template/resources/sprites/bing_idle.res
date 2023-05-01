# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: Sprite
deps: [ "res://textures/bing_idle_300x300_5" ]
description: "bing animation"
params:
  size_x: 150
  size_y: 150
  pivot_x: 0
  pivot_y: 0
  animations:
    - name: idle
      texture: "res://textures/bing_idle_300x300_5"
      speed: 4.0
      loop: true
      frames:
        - 0.0 0.0 0.2 1.0
        - 0.2 0.0 0.2 1.0
        - 0.4 0.0 0.2 1.0
        - 0.6 0.0 0.2 1.0
        - 0.8 0.0 0.2 1.0
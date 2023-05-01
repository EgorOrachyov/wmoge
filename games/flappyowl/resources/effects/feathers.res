# Wmoge resource meta file
version: 1
uuid: 0
loader: default
class: PfxEffect
deps:
  - "res://sprites/feather"
  - "res://shaders/sprite.shader"
description: ""
params:
  type: Effect2d
  components:
    - name: "feathers"
      amount: 20
      active: true
      features:
        - feature: PfxFeatureLifetime
          lifetime: 3.0
          infinite: false
        - feature: PfxFeatureMovement2d
          speed_acceleration: "0 -5"
          angle_acceleration: 1.0
        - feature: PfxFeatureSize
          start_size: 1
          end_size: 0
        - feature: PfxFeatureVelocity2d
          radius: 1.0
        - feature: PfxFeatureColor
          start_color: "1 1 1 0.6"
          end_color: "1 1 1 0"
        - feature: PfxFeatureSprite
          shader: "res://shaders/sprite.shader"
          sprite: "res://sprites/feather"
          animation: 0
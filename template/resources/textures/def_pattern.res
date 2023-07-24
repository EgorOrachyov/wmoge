# Wmoge resource meta file
version: 1
uuid: 0
loader: texture_2d
class: Texture2d
deps: [ ]
description: "default red texture"
params:
  source_file: "res://textures/def_pattern.png"
  channels: 4
  format: RGBA8
  mipmaps: true
  srgb: true
  compression: false
  sampling:
    min_lod: 0.0
    max_lod: 32.0
    max_anisotropy: 16.0
    min_flt: Linear
    mag_flt: LinearMipmapNearest
    u: Repeat
    v: Repeat
    w: Repeat
    brd_clr: Black
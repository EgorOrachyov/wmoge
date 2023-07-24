# Wmoge resource meta file
version: 1
uuid: 0
loader: texture_cube
class: TextureCube
deps: [ ]
description: "example skybox"
params:
  source_files:
    right: "res://textures/skybox_right.jpg"
    left: "res://textures/skybox_left.jpg"
    top: "res://textures/skybox_top.jpg"
    bottom: "res://textures/skybox_bottom.jpg"
    back: "res://textures/skybox_back.jpg"
    front: "res://textures/skybox_front.jpg"
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
    u: ClampToEdge
    v: ClampToEdge
    w: ClampToEdge
    brd_clr: Black
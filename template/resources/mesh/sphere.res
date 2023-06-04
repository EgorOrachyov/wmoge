# Wmoge resource meta file
version: 1
uuid: 0
loader: assimp
class: Mesh
deps: [ ]
description: "sphere test mesh"
params:
  source_file: "res://mesh/sphere.obj"
  attributes: [ "Pos3f", "Norm3f", "Tang3f", "Uv02f" ]
  process:
    triangulate: true
    tangent_space: true
    flip_uv: true
    gen_normals: false
    gen_smooth_normals: true
    join_identical_vertices: true
    limit_bone_weights: true
    improve_cache_locality: true
    sort_by_ptype: true
    gen_uv: true



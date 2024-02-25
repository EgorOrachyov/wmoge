from reflection import *
from common import *

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_MAT4, "GridProj"),
    StructField(TYPE_MAT4, "GridProjInv"),
    StructField(TYPE_MAT4, "GridView"),
    StructField(TYPE_MAT4, "GridViewInv"),
    StructField(TYPE_VEC4, "GridDim"),
    StructField(TYPE_VEC4, "GridDimInv"),
    StructField(TYPE_UINT, "GridCellSize"),
    StructField(TYPE_UINT, "GridSize"),
    StructField(TYPE_UINT, "GridNumLights"),
]

SHADER = Shader(
    name="light_grid_build",
    cls="LightGridBuild",
    structs=[
        Light,
    ],
    buffers=[
        UniformBuffer("Params", "std140", BINDINGS.next(), Params),
        StorageBuffer("LightsData", "std430", BINDINGS.next(), Lights),
        StorageBuffer("LightGridSizes", "std430", BINDINGS.next(), LightGridSizes),
        StorageBuffer("LightGridLights", "std430", BINDINGS.next(), LightGridLights),
    ],
    files=["light_grid_build.comp"],
)

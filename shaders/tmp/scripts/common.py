from reflection import *

FrameData = [
    StructField(TYPE_FLOAT, "time"),
    StructField(TYPE_FLOAT, "timeDelta"),
    StructField(TYPE_FLOAT, "_fd_pad0"),
    StructField(TYPE_FLOAT, "_fd_pad1"),
]

ViewData = [
    StructField(TYPE_MAT4, "Clip"),
    StructField(TYPE_MAT4, "Proj"),
    StructField(TYPE_MAT4, "View"),
    StructField(TYPE_MAT4, "ProjView"),
    StructField(TYPE_MAT4, "ClipProjView"),
    StructField(TYPE_MAT4, "ProjPrev"),
    StructField(TYPE_MAT4, "ViewPrev"),
    StructField(TYPE_MAT4, "ProjViewPrev"),
    StructField(TYPE_MAT4, "ClipProjViewPrev"),
    StructField(TYPE_VEC4, "Movement"),
    StructField(TYPE_VEC4, "Position"),
    StructField(TYPE_VEC4, "Direction"),
    StructField(TYPE_VEC4, "Up"),
    StructField(TYPE_VEC4, "PositionPrev"),
    StructField(TYPE_VEC4, "DirectionPrev"),
    StructField(TYPE_VEC4, "UpPrev"),
    StructField(TYPE_IVEC4, "Viewport"),
    StructField(TYPE_INT, "CamIdx"),
    StructField(TYPE_INT, "_vd_pad0"),
    StructField(TYPE_INT, "_vd_pad1"),
    StructField(TYPE_INT, "_vd_pad2"),
]

RenderObjectData = Struct(
    "RenderObjectData",
    [
        StructField(TYPE_MAT4, "LocalToWorld"),
        StructField(TYPE_MAT4, "LocalToWorldPrev"),
        StructField(TYPE_MAT4, "NormalMatrix"),
        StructField(TYPE_VEC4, "AabbPos"),
        StructField(TYPE_VEC4, "AabbSizeHalf"),
    ],
)

Light = Struct(
    "LightData",
    [
        StructField(TYPE_MAT4, "ViewProj"),
        StructField(TYPE_VEC4, "WorldPos"),
        StructField(TYPE_VEC4, "WorldDir"),
        StructField(TYPE_VEC4, "Color"),
        StructField(TYPE_FLOAT, "Intensity"),
        StructField(TYPE_FLOAT, "Radius"),
        StructField(TYPE_FLOAT, "CosInner"),
        StructField(TYPE_FLOAT, "CosOuter"),
        StructField(TYPE_INT, "Type"),
        StructField(TYPE_INT, "_ld_pad0"),
        StructField(TYPE_INT, "_ld_pad1"),
        StructField(TYPE_INT, "_ld_pad2"),
    ],
)

Lights = [
    StructField(Light, "Lights", ArrayQualifier()),
]

LightGridSizes = [
    StructField(TYPE_UINT, "LightGridCellSizes", ArrayQualifier()),
]

LightGridLights = [
    StructField(TYPE_UINT, "LightGridCellLights", ArrayQualifier()),
]

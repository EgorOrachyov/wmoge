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
        StructField(TYPE_MAT4, "WorldToLocal"),
        StructField(TYPE_MAT4, "WorldToLocalPrev"),
        StructField(TYPE_MAT4, "NormalMatrix"),
        StructField(TYPE_VEC4, "AabbPos"),
        StructField(TYPE_VEC4, "AabbSizeHalf"),
    ],
)

BINDINGS = BindingAllocator()

SHADER = Shader(
    name="material",
    cls="Material",
    constants=[],
    structs=[
        RenderObjectData,
    ],
    buffers=[
        UniformBuffer(
            "FrameData",
            "std140",
            BINDINGS.next(),
            FrameData,
        ),
        UniformBuffer(
            "ViewData",
            "std140",
            BINDINGS.next(),
            ViewData,
        ),
        StorageBuffer(
            "RenderObjectsData",
            "std430",
            BINDINGS.next(0),
            [StructField(RenderObjectData, "RenderObjects", ArrayQualifier())],
            readonly=True,
        ),
    ],
    samplers=[],
    files=["material.vert", "material.frag"],
    material_set=1,
)

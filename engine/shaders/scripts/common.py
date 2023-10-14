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

DrawCallData = [
    StructField(TYPE_MAT4, "Model"),
    StructField(TYPE_MAT4, "ModelPrev"),
    StructField(TYPE_VEC4, "AabbPos"),
    StructField(TYPE_VEC4, "AabbSizeHalf"),
]

BINDINGS = BindingAllocator()

SHADER = Shader(
    name="common",
    cls="Common",
    constants=[],
    buffers=[
        UniformBuffer(
            "FrameData",
            "std140",
            BINDINGS.next(0),
            FrameData,
        ),
        UniformBuffer(
            "ViewData",
            "std140",
            BINDINGS.next(0),
            ViewData,
        ),
        UniformBuffer(
            "DrawCallData",
            "std140",
            BINDINGS.next(2),
            DrawCallData,
        ),
    ],
    samplers=[],
    files=[],
    material_set=1,
)

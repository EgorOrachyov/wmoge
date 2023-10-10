from reflection import *

BINDINGS = BindingAllocator()

SHADER = Shader(
    name="material_deferred",
    cls="MaterialDeferred",
    constants=[],
    buffers=[
        UniformBuffer(
            "FrameData",
            "std140",
            BINDINGS.next(),
            [
                StructField(TYPE_FLOAT, "time"),
                StructField(TYPE_FLOAT, "timeDelta"),
                StructField(TYPE_FLOAT, "_pad0"),
                StructField(TYPE_FLOAT, "_pad1"),
            ],
        ),
        UniformBuffer(
            "ViewData",
            "std140",
            BINDINGS.next(),
            [
                StructField(TYPE_MAT4, "Proj"),
                StructField(TYPE_MAT4, "View"),
                StructField(TYPE_MAT4, "ProjView"),
                StructField(TYPE_MAT4, "ProjPrev"),
                StructField(TYPE_MAT4, "ViewPrev"),
                StructField(TYPE_MAT4, "ProjViewPrev"),
            ],
        ),
        UniformBuffer(
            "DrawCallData",
            "std140",
            BINDINGS.next(2),
            [
                StructField(TYPE_MAT4, "Model"),
                StructField(TYPE_MAT4, "ModelPrev"),
            ],
        ),
    ],
    samplers=[],
    files=["material_deferred.vert", "material_deferred.frag"],
    material_set=1,
)

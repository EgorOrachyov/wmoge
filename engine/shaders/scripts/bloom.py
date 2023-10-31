from reflection import *

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_VEC4, "ThresholdKnee"),
    StructField(TYPE_FLOAT, "UpsampleRadius"),
    StructField(TYPE_FLOAT, "UpsampleWeight"),
    StructField(TYPE_FLOAT, "_pr_pad0"),
    StructField(TYPE_FLOAT, "_pr_pad1"),
]

SHADER = Shader(
    name="bloom",
    cls="Bloom",
    constants=[],
    structs=[],
    buffers=[
        UniformBuffer(
            "Params",
            "std140",
            BINDINGS.next(),
            Params,
        )
    ],
    samplers=[
        Sampler2d("Source", BINDINGS.next()),
        Sampler2d("SourcePrev", BINDINGS.next()),
    ],
    images=[Image2D("Result", BINDINGS.next(), "rgba16f", "writeonly")],
    files=["bloom.comp"],
)

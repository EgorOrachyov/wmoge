from reflection import *

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_MAT4, "Clip"),
    StructField(TYPE_FLOAT, "InverseGamma"),
    StructField(TYPE_FLOAT, "Mode"),
    StructField(TYPE_FLOAT, "Exposure"),
    StructField(TYPE_FLOAT, "_pr_pad0"),
]

SHADER = Shader(
    name="tonemap",
    cls="Tonemap",
    constants=[Constant("TONEMAP_MODE_EXP", "0")],
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
        Sampler2d("Image", BINDINGS.next()),
    ],
    files=["tonemap.vert", "tonemap.frag"],
)

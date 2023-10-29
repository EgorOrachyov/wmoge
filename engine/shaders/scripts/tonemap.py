from reflection import *

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_MAT4, "Clip"),
    StructField(TYPE_FLOAT, "InverseGamma"),
    StructField(TYPE_FLOAT, "Mode"),
    StructField(TYPE_FLOAT, "Exposure"),
    StructField(TYPE_FLOAT, "BloomIntensity"),
    StructField(TYPE_FLOAT, "BloomDirtMaskIntensity"),
    StructField(TYPE_FLOAT, "WhitePoint"),
    StructField(TYPE_FLOAT, "_pd_pad0"),
    StructField(TYPE_FLOAT, "_pd_pad1"),
]

SHADER = Shader(
    name="tonemap",
    cls="Tonemap",
    constants=[
        Constant("TONEMAP_MODE_EXP", "0"),
        Constant("TONEMAP_MODE_REINH", "1"),
        Constant("TONEMAP_MODE_REINH_EXT", "2"),
        Constant("TONEMAP_MODE_ACES", "3"),
        Constant("TONEMAP_MODE_UN2", "4"),
    ],
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
        Sampler2d("Bloom", BINDINGS.next()),
        Sampler2d("BloomDirtMask", BINDINGS.next()),
    ],
    files=["tonemap.vert", "tonemap.frag"],
)

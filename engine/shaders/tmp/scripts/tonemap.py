from reflection import *
from luminance import Luminance

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_FLOAT, "InverseGamma"),
    StructField(TYPE_FLOAT, "Mode"),
    StructField(TYPE_FLOAT, "Exposure"),
    StructField(TYPE_FLOAT, "BloomIntensity"),
    StructField(TYPE_FLOAT, "BloomDirtMaskIntensity"),
    StructField(TYPE_FLOAT, "WhitePoint"),
    StructField(TYPE_FLOAT, "_pr_pad0"),
    StructField(TYPE_FLOAT, "_pr_pad1"),
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
        UniformBuffer("Params", "std140", BINDINGS.next(), Params),
        StorageBuffer("Luminance", "std430", BINDINGS.next(), Luminance, readonly=True),
    ],
    samplers=[
        Sampler2d("Image", BINDINGS.next()),
        Sampler2d("Bloom", BINDINGS.next()),
        Sampler2d("BloomDirtMask", BINDINGS.next()),
    ],
    images=[Image2D("Result", BINDINGS.next(), "rgba8", "writeonly")],
    files=["tonemap.comp"],
)

from reflection import *
from luminance import *

BINDINGS = BindingAllocator()

SHADER = Shader(
    name="luminance_avg",
    cls="LuminanceAvg",
    constants=[
        Constant("NUM_HISTOGRAM_BINS", f"{NUM_HISTOGRAM_BINS}"),
        Constant("MODE_ADAPTIVE", "0"),
        Constant("MODE_INSTANT", "1"),
    ],
    buffers=[
        UniformBuffer("Params", "std140", BINDINGS.next(), Params),
        StorageBuffer("Histogram", "std430", BINDINGS.next(), Histogram),
        StorageBuffer("Luminance", "std430", BINDINGS.next(), Luminance),
    ],
    samplers=[Sampler2d("Image", BINDINGS.next())],
    files=["luminance_avg.comp"],
)

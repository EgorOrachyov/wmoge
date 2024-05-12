from reflection import *
from luminance import *

BINDINGS = BindingAllocator()
NUM_HISTOGRAM_BINS = 256

SHADER = Shader(
    name="luminance_histogram",
    cls="LuminanceHistogram",
    constants=[
        Constant("NUM_HISTOGRAM_BINS", f"{NUM_HISTOGRAM_BINS}"),
    ],
    buffers=[
        UniformBuffer("Params", "std140", BINDINGS.next(), Params),
        StorageBuffer("Histogram", "std430", BINDINGS.next(), Histogram),
        StorageBuffer("Luminance", "std430", BINDINGS.next(), Luminance),
    ],
    samplers=[Sampler2d("Image", BINDINGS.next())],
    files=["luminance_histogram.comp"],
)

from reflection import *
from common import FrameData, ViewData, DrawCallData

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
            FrameData,
        ),
        UniformBuffer(
            "ViewData",
            "std140",
            BINDINGS.next(),
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
    files=["material_deferred.vert", "material_deferred.frag"],
    material_set=1,
)

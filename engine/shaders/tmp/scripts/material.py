from reflection import *
from common import *

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
            BINDINGS.next(),
            [StructField(RenderObjectData, "RenderObjects", ArrayQualifier())],
            readonly=True,
        ),
    ],
    samplers=[],
    files=["material.vert", "material.frag"],
    material_set=1,
)

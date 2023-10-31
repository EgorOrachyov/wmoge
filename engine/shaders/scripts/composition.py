from reflection import *

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_MAT4, "Clip"),
    StructField(TYPE_UVEC2, "TargetSize"),
    StructField(TYPE_FLOAT, "Gamma"),
    StructField(TYPE_FLOAT, "InverseGamma"),
]

SHADER = Shader(
    name="composition",
    cls="Composition",
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
        Sampler2d("Color", BINDINGS.next()),
    ],
    images=[],
    files=["composition.vert", "composition.frag"],
)

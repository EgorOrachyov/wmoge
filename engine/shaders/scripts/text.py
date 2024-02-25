from reflection import *

BINDINGS = BindingAllocator()

SHADER = Shader(
    name="text",
    cls="Text",
    constants=[],
    structs=[],
    buffers=[
        UniformBuffer(
            "Params",
            "std140",
            BINDINGS.next(),
            [
                StructField(TYPE_MAT4, "mat_clip_proj_screen"),
                StructField(TYPE_FLOAT, "inverse_gamma"),
                StructField(TYPE_FLOAT, "_tp_pad0"),
                StructField(TYPE_FLOAT, "_tp_pad1"),
                StructField(TYPE_FLOAT, "_tp_pad2"),
            ],
        )
    ],
    samplers=[Sampler2d("FontTexture", BINDINGS.next())],
    files=["text.vert", "text.frag"],
)

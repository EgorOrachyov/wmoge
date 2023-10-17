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
                StructField(TYPE_FLOAT, "__pad_1"),
                StructField(TYPE_FLOAT, "__pad_2"),
                StructField(TYPE_FLOAT, "__pad_3"),
            ],
        )
    ],
    samplers=[Sampler2d("FontTexture", BINDINGS.next())],
    files=["text.vert", "text.frag"],
)

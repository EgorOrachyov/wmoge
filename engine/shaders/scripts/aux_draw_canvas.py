from reflection import *

BINDINGS = BindingAllocator()

SHADER = Shader(
    name="aux_draw_canvas",
    cls="AuxDrawCanvas",
    constants=[],
    buffers=[
        UniformBuffer(
            "Params",
            "std140",
            BINDINGS.next(),
            [
                StructField(TYPE_MAT4, "clip_proj_screen"),
                StructField(TYPE_FLOAT, "gamma"),
                StructField(TYPE_FLOAT, "inverse_gamma"),
            ],
        )
    ],
    samplers=[Sampler2d("Texture", BINDINGS.next())],
    files=["aux_draw_canvas.vert", "aux_draw_canvas.frag"],
)

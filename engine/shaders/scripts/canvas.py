from reflection import *

BINDINGS = BindingAllocator()

Params = [
    StructField(TYPE_MAT4, "ClipProjView"),
    StructField(TYPE_FLOAT, "InverseGamma"),
    StructField(TYPE_FLOAT, "_pr_pad0"),
    StructField(TYPE_FLOAT, "_pr_pad1"),
    StructField(TYPE_FLOAT, "_pr_pad2"),
]

DrawCmdData = Struct(
    "DrawCmdData",
    [
        StructField(TYPE_VEC4, "Transform0"),
        StructField(TYPE_VEC4, "Transform1"),
        StructField(TYPE_VEC4, "Transform2"),
        StructField(TYPE_VEC4, "ClipRect"),
        StructField(TYPE_INT, "TextureIdx"),
        StructField(TYPE_INT, "_dc_pad0"),
        StructField(TYPE_INT, "_dc_pad1"),
        StructField(TYPE_INT, "_dc_pad2"),
    ],
)

SHADER = Shader(
    name="canvas",
    cls="Canvas",
    constants=[Constant("MAX_CANVAS_IMAGES", "4")],
    structs=[DrawCmdData],
    buffers=[
        UniformBuffer(
            "Params",
            "std140",
            BINDINGS.next(0),
            Params,
        ),
        StorageBuffer(
            "DrawCmdsData",
            "std430",
            BINDINGS.next(0),
            [StructField(DrawCmdData, "DrawCmds", ArrayQualifier())],
            readonly=True,
        ),
    ],
    samplers=[
        Sampler2d("CanvasImage0", BINDINGS.next(1)),
        Sampler2d("CanvasImage1", BINDINGS.next(1)),
        Sampler2d("CanvasImage2", BINDINGS.next(1)),
        Sampler2d("CanvasImage3", BINDINGS.next(1)),
    ],
    files=["canvas.vert", "canvas.frag"],
)

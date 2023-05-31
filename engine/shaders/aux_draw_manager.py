from reflection import *

BINDINGS = BindingAllocator()

SHADER = Shader(name='aux_draw_manager',
                cls='AuxDrawManager',
                constants=
                [

                ],
                buffers=
                [
                    UniformBuffer('Params', 'std430', BINDINGS.next(), [
                        StructField(TYPE_MAT4, 'clip_proj_view'),
                        StructField(TYPE_MAT4, 'clip_proj_screen'),
                        StructField(TYPE_FLOAT, 'gamma'),
                        StructField(TYPE_FLOAT, 'inverse_gamma')
                    ])
                ],
                samplers=
                [
                    Sampler2d('FontBitmap', BINDINGS.next())
                ],
                files=
                [
                    'aux_draw_manager.vert',
                    'aux_draw_manager.frag'
                ]
                )

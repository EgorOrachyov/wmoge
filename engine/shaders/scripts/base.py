from reflection import *

BINDINGS = BindingAllocator()

SHADER = Shader(name='base',
                cls='Base',
                constants=
                [

                ],
                buffers=
                [
                    UniformBuffer('Params', 'std140', BINDINGS.next(), [
                        StructField(TYPE_MAT4, 'mat_clip_proj_view'),
                        StructField(TYPE_VEC4, 'base_color'),
                        StructField(TYPE_FLOAT, 'inverse_gamma'),
                        StructField(TYPE_FLOAT, 'mix_weight_1'),
                        StructField(TYPE_FLOAT, 'mix_weight_2'),
                        StructField(TYPE_FLOAT, 'mix_weight_3')
                    ])
                ],
                samplers=
                [

                ],
                files=
                [
                    'base.vert',
                    'base.frag'
                ]
                )

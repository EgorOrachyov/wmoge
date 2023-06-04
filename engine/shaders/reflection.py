class Type:
    def __init__(self, name):
        self.name = name


class TypeScalar(Type):
    def __init__(self, name, size):
        Type.__init__(self, name)
        self.size = size


class TypeComposite(Type):
    def __init__(self, name, base_type):
        Type.__init__(self, name)
        self.base_type = base_type


class TypeVector(TypeComposite):
    def __init__(self, name, base_type, components):
        TypeComposite.__init__(self, name, base_type)
        self.components = components


class TypeMatrix(TypeComposite):
    def __init__(self, name, base_type, rows, cols):
        TypeComposite.__init__(self, name, base_type)
        self.rows = rows
        self.cols = cols


class TypeSampler(Type):
    def __init__(self, name):
        Type.__init__(self, name)


TYPE_FLOAT = TypeScalar('float', 4)
TYPE_INT = TypeScalar('int', 4)
TYPE_UINT = TypeScalar('uint', 4)
TYPE_VEC2 = TypeVector('vec2', TYPE_FLOAT, 2)
TYPE_VEC3 = TypeVector('vec2', TYPE_FLOAT, 3)
TYPE_VEC4 = TypeVector('vec2', TYPE_FLOAT, 4)
TYPE_IVEC2 = TypeVector('ivec2', TYPE_INT, 2)
TYPE_IVEC3 = TypeVector('ivec2', TYPE_INT, 3)
TYPE_IVEC4 = TypeVector('ivec2', TYPE_INT, 4)
TYPE_UVEC2 = TypeVector('uvec2', TYPE_UINT, 2)
TYPE_UVEC3 = TypeVector('uvec2', TYPE_UINT, 3)
TYPE_UVEC4 = TypeVector('uvec2', TYPE_UINT, 4)
TYPE_MAT2 = TypeMatrix('mat2', TYPE_FLOAT, 2, 2)
TYPE_MAT3 = TypeMatrix('mat3', TYPE_FLOAT, 3, 3)
TYPE_MAT4 = TypeMatrix('mat4', TYPE_FLOAT, 4, 4)

TYPE_SAMPLER_2D = TypeSampler('sampler2D')
TYPE_SAMPLER_3D = TypeSampler('sampler3D')
TYPE_SAMPLER_CUBE = TypeSampler('samplerCube')


class Binding:
    def __init__(self, set_num, slot_num):
        self.set_num = set_num
        self.slot_num = slot_num

    def to_gfx_loc(self):
        return "GfxLocation{" + str(self.set_num) + "," + str(self.slot_num) + "}"


class BindingAllocator:
    def __init__(self, max_sets=3):
        self.max_sets = max_sets
        self.next_slot = 0

    def next(self, set_num=0):
        slot_num = self.next_slot
        self.next_slot += 1
        return Binding(set_num, slot_num)


class StructField:
    def __init__(self, decl_type: Type, name: str, array_size=None):
        self.decl_type = decl_type
        self.name = name
        self.array_size = array_size

    def is_array(self):
        return self.array_size is not None


class Struct(Type):
    def __init__(self, name, fields: list[StructField]):
        Type.__init__(self, name)
        self.fields = fields


class Buffer:
    def __init__(self, name, layout, binding, struct, keyword):
        self.name = name
        self.layout = layout
        self.binding = binding
        self.struct = struct
        self.keyword = keyword

    def gen_decl(self, lang):
        return self.struct.gen_decl(self.name, self.binding, lang, self.keyword)


class UniformBuffer(Buffer):
    def __init__(self, name, layout, binding, fields):
        Buffer.__init__(self, name, layout, binding, Struct(name, fields), 'uniform')


class StorageBuffer(Buffer):
    def __init__(self, name, layout, binding, fields):
        Buffer.__init__(self, name, layout, binding, Struct(name, fields), 'buffer')


class Constant:
    def __init__(self, name, value):
        self.name = name
        self.value = value


class Sampler:
    def __init__(self, name, binding, decl_type):
        self.name = name
        self.binding = binding
        self.decl_type = decl_type


class Sampler2d(Sampler):
    def __init__(self, name, binding):
        Sampler.__init__(self, name, binding, TYPE_SAMPLER_2D)


class Sampler3d(Sampler):
    def __init__(self, name, binding):
        Sampler.__init__(self, name, binding, TYPE_SAMPLER_3D)


class SamplerCube(Sampler):
    def __init__(self, name, binding):
        Sampler.__init__(self, name, binding, TYPE_SAMPLER_CUBE)


class Shader:
    def __init__(self,
                 name: str,
                 cls: str,
                 constants: list[Constant],
                 buffers: list[Buffer],
                 samplers: list[Sampler],
                 files: list[str]):
        self.name = name
        self.cls = cls
        self.constants = constants
        self.buffers = buffers
        self.samplers = samplers
        self.files = files

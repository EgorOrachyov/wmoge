##################################################################################
# Wmoge game engine                                                              #
# Available at github https://github.com/EgorOrachyov/wmoge                      #
##################################################################################
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2023 Egor Orachyov                                               #
#                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy   #
# of this software and associated documentation files (the "Software"), to deal  #
# in the Software without restriction, including without limitation the rights   #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      #
# copies of the Software, and to permit persons to whom the Software is          #
# furnished to do so, subject to the following conditions:                       #
#                                                                                #
# The above copyright notice and this permission notice shall be included in all #
# copies or substantial portions of the Software.                                #
#                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  #
# SOFTWARE.                                                                      #
##################################################################################

import yaml
import argparse

header = "// auto generated file\n\n#pragma once\n"

includes_hpp = '#include "grc/shader.hpp"\n'

includes_cpp = (
    '#include "grc/shader_manager.hpp"\n#include "grc/shader_reflection.hpp"\n'
)

namespace = "wmoge"

files_list = ["aux_draw", "blit", "canvas", "fill"]

types_map = {
    "vec2": "Vec2f",
    "vec3": "Vec3f",
    "vec4": "Vec4f",
    "ivec2": "Vec2i",
    "ivec3": "Vec3i",
    "ivec4": "Vec4i",
    "uvec2": "Vec2u",
    "uvec3": "Vec3u",
    "uvec4": "Vec4u",
    "int": "int",
    "float": "float",
}

types_size = {
    "vec2": 8,
    "vec3": 12,
    "vec4": 16,
    "ivec2": 8,
    "ivec3": 12,
    "ivec4": 16,
    "uvec2": 8,
    "uvec3": 12,
    "uvec4": 16,
    "int": 4,
    "float": 4,
}

reflectable = []


def to_camel_case(name: str):
    return "".join([word[0:1].upper() + word[1:] for word in name.split("_")])


def calc_sizeof(fields):
    sizeof = 0

    for field in fields:
        array_size = 1
        if "array_size" in field:
            array_size = field["array_size"]
            if array_size == -1:
                continue
        sizeof += array_size * types_size[field["type"]]

    return sizeof


def process_field(builder_hpp, builder_cpp, field):
    field_name = field["name"]
    field_type = field["type"]
    array_size = 1
    is_array = False
    is_unbound = False

    if "array_size" in field:
        array_size = field["array_size"]
        is_array = True
        if array_size == -1:
            is_unbound = True

    if is_array:
        if not is_unbound:
            builder_hpp += f"{types_map[field_type]} {field_name}[{array_size}];"
        else:
            builder_hpp += f"// {types_map[field_type]} {field_name}[];"
        builder_cpp += f'r.add_field_array(Strid("{field_name}"), Strid("{field_type}")'
        if not is_unbound:
            builder_cpp += f", {array_size}"
        builder_cpp += ");"
    else:
        builder_hpp += f"{types_map[field_type]} {field_name};"
        builder_cpp += f'r.add_field(Strid("{field_name}"), Strid("{field_type}"));'

    builder_hpp += "\n"
    builder_cpp += "\n"

    return builder_hpp, builder_cpp


def process_struct(builder_hpp, builder_cpp, struct):
    struct_name = struct["name"]

    types_map[struct_name] = struct_name

    builder_hpp += f"/// @brief Auto-generated reflection for struct '{struct_name}'\n"
    builder_hpp += f"struct {struct_name}" + "{\n"

    sizeof = calc_sizeof(struct["fields"])

    builder_cpp += f"Status {struct_name}::reflect(class ShaderManager* sm)" + "{\n"
    builder_cpp += f'ShaderStructRegister r(Strid("{struct_name}"), {sizeof}, sm);\n'

    for field in struct["fields"]:
        builder_hpp, builder_cpp = process_field(builder_hpp, builder_cpp, field)

    builder_cpp += "return r.finish();\n"
    builder_cpp += "}\n\n"

    builder_hpp += "\nstatic Status reflect(class ShaderManager* sm);\n"
    builder_hpp += "};\n\n"

    types_size[struct_name] = sizeof
    reflectable.append(struct_name)

    return builder_hpp, builder_cpp


def process_structs(builder_hpp, builder_cpp, structs):
    for struct in structs:
        builder_hpp, builder_cpp = process_struct(builder_hpp, builder_cpp, struct)

    return builder_hpp, builder_cpp


def process_option(builder, option):
    option_name = option["name"]
    builder += f'const Strid {option_name.lower()} = SID("{option_name}");\n'

    for variant in option["variants"]:
        builder += (
            f"const ShaderOptionVariant {option_name.lower()}_{variant.lower()} = "
            + "{"
            + f'SID("{option_name}"), SID("{variant}")'
            + "};\n"
        )

    builder += "\n"
    return builder


def process_options(builder, options):
    builder += "struct Options {\n"

    for option in options:
        builder = process_option(builder, option)
        builder += "\n"

    builder += "} options;\n"
    return builder


def process_pass(builder, shader_pass):
    pass_name = shader_pass["name"]

    builder += f"struct Pass{to_camel_case(pass_name)} " + "{\n"
    builder += f'const Strid name = SID("{pass_name}");\n\n'

    if "options" in shader_pass:
        builder = process_options(builder, shader_pass["options"])

    builder += "operator Strid() const { return name; }\n\n"

    builder += "}" + f" ps_{pass_name};\n\n"
    return builder


def process_technique(builder, shader_technique):
    technique_name = shader_technique["name"]

    builder += f"struct Technique{to_camel_case(technique_name)} " + "{\n"
    builder += f'const Strid name = SID("{technique_name}");\n\n'

    if "options" in shader_technique:
        builder = process_options(builder, shader_technique["options"])
        builder += "\n"

    for shader_pass in shader_technique["passes"]:
        builder = process_pass(builder, shader_pass)

    builder += "operator Strid() const { return name; }\n\n"

    builder += "}" + f" tq_{technique_name};\n\n"
    return builder


def process_param(builder_hpp, builder_cpp, param):
    param_name = param["name"]
    param_name_lower = param_name.lower()

    builder_hpp += f'const Strid {param_name_lower}_name = SID("{param_name}");\n'
    builder_hpp += f"ShaderParamId {param_name_lower};\n"
    builder_hpp += "\n"

    builder_cpp += (
        f"{param_name_lower} = shader->find_param_id({param_name_lower}_name);\n"
    )

    return builder_hpp, builder_cpp


def process_param_block(builder_hpp, builder_cpp, param_block, shader_class_name):
    param_block_name = param_block["name"]
    param_block_class_name = f"ParamBlock{to_camel_case(param_block_name)}"

    builder_hpp += f"struct {param_block_class_name} " + "{\n"
    builder_hpp += f'const Strid name = SID("{param_block_name}");\n\n'

    builder_cpp += (
        f"void {shader_class_name}::{param_block_class_name}::load_from(const Ref<Shader>& shader) "
        + "{\n"
    )

    for param in param_block["params"]:
        builder_hpp, builder_cpp = process_param(builder_hpp, builder_cpp, param)

    builder_cpp += "}\n\n"
    builder_hpp += "void load_from(const Ref<Shader>& s);\n"

    builder_hpp += "}" + f" pb_{param_block_name};\n\n"
    return builder_hpp, builder_cpp


def process_constants(builder, constants):
    builder += "struct Constants {\n"

    for constant in constants:
        builder += f'static constexpr {constant["type"]} {constant["name"]} = {constant["value"].__repr__()};\n'

    builder += "};\n\n"
    return builder


def open_shader(shader_path):
    with open(shader_path, "r") as file:
        return yaml.safe_load(file)
    return None


def process_shader(builder_hpp, builder_cpp, shader):
    if "structs" in shader:
        builder_hpp, builder_cpp = process_structs(
            builder_hpp, builder_cpp, shader["structs"]
        )

    shader_name = shader["name"]
    shader_class_name = f"Shader{to_camel_case(shader_name)}"

    builder_hpp += f"/// @brief Auto-generated reflection for '{shader_name}' shader\n"
    builder_hpp += f"struct {shader_class_name} " + "{\n"

    if "constants" in shader:
        builder_hpp = process_constants(builder_hpp, shader["constants"])

    for param_block in shader["param_blocks"]:
        builder_hpp, builder_cpp = process_param_block(
            builder_hpp, builder_cpp, param_block, shader_class_name
        )

    for shader_technique in shader["techniques"]:
        builder_hpp = process_technique(builder_hpp, shader_technique)

    builder_hpp += "Ref<Shader> shader;\n\n"
    builder_hpp += "operator Shader*() const { return shader.get(); }\n\n"

    builder_hpp += f"Status load_from(Ref<Shader> s);\n"

    builder_cpp += f"Status {shader_class_name}::load_from(Ref<Shader> s) " + "{\n"
    builder_cpp += "shader = std::move(s);\n"

    for param_block in shader["param_blocks"]:
        builder_cpp += f'pb_{param_block["name"]}.load_from(shader);\n'

    builder_cpp += "return WG_OK;\n"
    builder_cpp += "}\n\n"

    builder_hpp += "};\n\n"

    return builder_hpp, builder_cpp


def process_reflect(builder_hpp, builder_cpp):
    builder_hpp += "Status reflect_shader_types(class ShaderManager* sm);\n\n"

    builder_cpp += "Status reflect_shader_types(class ShaderManager* sm) {\n"

    for type in reflectable:
        builder_cpp += f"WG_CHECKED({type}::reflect(sm));\n"

    builder_cpp += "return WG_OK;\n"
    builder_cpp += "};\n\n"

    return builder_hpp, builder_cpp


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--input-dir", default="./engine/shaders/", help="path to dir with shaders"
    )
    parser.add_argument(
        "--output-dir",
        default="./engine/runtime/render/",
        help="path to dir for output",
    )
    parser.add_argument(
        "--out-file",
        default="interop",
        help="output file name for generated code",
    )

    args = parser.parse_args()

    files_list = [file + ".shader" for file in files_list]

    shaders = []
    for shader_file in files_list:
        shaders.append(open_shader(args.input_dir + shader_file))

    builder_hpp = ""
    builder_cpp = ""

    for shader in shaders:
        builder_hpp, builder_cpp = process_shader(builder_hpp, builder_cpp, shader)

    builder_hpp, builder_cpp = process_reflect(builder_hpp, builder_cpp)

    with open(args.output_dir + args.out_file + ".hpp", "w") as file:
        file.write(header)
        file.write("\n")
        file.write(includes_hpp)
        file.write("\n")
        file.write(f"namespace {namespace}" + "{\n")
        file.write("\n")
        file.write(builder_hpp)
        file.write("\n}" + f"// namespace {namespace}")

    with open(args.output_dir + args.out_file + ".cpp", "w") as file:
        file.write(header)
        file.write("\n")
        file.write(f'#include "{args.out_file}.hpp"\n')
        file.write("\n")
        file.write(includes_cpp)
        file.write("\n")
        file.write(f"namespace {namespace}" + "{\n")
        file.write("\n")
        file.write(builder_cpp)
        file.write("\n}" + f"// namespace {namespace}")

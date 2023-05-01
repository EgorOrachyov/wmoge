/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#include "shader_2d.hpp"

#include "core/class.hpp"
#include "shaders/generated/auto_shader_2d_fs.hpp"
#include "shaders/generated/auto_shader_2d_params_fs.hpp"
#include "shaders/generated/auto_shader_2d_params_vs.hpp"
#include "shaders/generated/auto_shader_2d_vs.hpp"

namespace wmoge {

    bool Shader2d::load_from_import_options(const YamlTree& tree) {
        return MaterialShader::load_from_import_options(tree);
    }

    void Shader2d::copy_to(Resource& copy) {
        MaterialShader::copy_to(copy);
    }

    void Shader2d::on_build_add_defines(ShaderBuilder& builder) {
        MaterialShader::on_build_add_defines(builder);

        builder.add_define("SHADER_2D");
        builder.add_define("MAT_BINDING_PARAMS 2");
        builder.add_define("MAT_BINDING_TEX 3");
        builder.set_material_bindings(2, 3);
    }
    void Shader2d::on_build_add_params(ShaderBuilder& builder) {
        MaterialShader::on_build_add_params(builder);

        std::string common_params =
                "layout (set = DRAW_SET_PER_PASS, binding = 0) uniform CommonParameters {\n"
                "    mat4 clip_proj_view;\n"
                "    mat4 clip_proj_view_prev;\n"
                "    vec2 view_size;\n"
                "    float time;\n"
                "    float time_dt;\n"
                "};\n\n";

        std::string draw_params =
                "layout (set = DRAW_SET_PER_DRAW, binding = 1) uniform DrawParameters {\n"
                "   mat4 model;\n"
                "   mat4 model_prev;\n"
                "   mat4 model_inv;\n"
                "   mat4 model_inv_prev;\n"
                "   vec4 tint;\n"
                "   int layer_id;\n"
                "   int _wg_pad_0;\n"
                "   int _wg_pad_1;\n"
                "   int _wg_pad_2;\n"
                "};\n\n";

        builder
                .add_vertex_module(draw_params)
                .add_fragment_module(draw_params)
                .add_vertex_module(common_params)
                .add_fragment_module(common_params)
                .add_vertex_module(source_shader_2d_params_vs)
                .add_fragment_module(source_shader_2d_params_fs);
    }
    void Shader2d::on_build_add_main(ShaderBuilder& builder) {
        MaterialShader::on_build_add_main(builder);

        const char* vsout = "layout (location = 0) out vec2 fs_uv;\n"
                            "layout (location = 1) out vec4 fs_color;\n";
        const char* fsin  = "layout (location = 0) in vec2 fs_uv;\n"
                            "layout (location = 1) in vec4 fs_color;\n";

        builder
                .add_vertex_module(vsout)
                .add_fragment_module(fsin)
                .add_vertex_module(source_shader_2d_vs)
                .add_fragment_module(source_shader_2d_fs);
    }

    void Shader2d::register_class() {
        auto* cls = Class::register_class<Shader2d>();
    }

}// namespace wmoge
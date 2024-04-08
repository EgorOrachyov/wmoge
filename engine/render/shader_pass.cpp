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

#include "shader_pass.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "debug/profiler.hpp"
#include "render/shader_builder.hpp"

namespace wmoge {

    Status ShaderPass::compile(const Strid& name, GfxDriver* driver, const GfxVertAttribs& attribs, const buffered_vector<std::string>& defines, class Shader* shader, Ref<GfxShader>& out_shader) {
        WG_AUTO_PROFILE_RENDER("ShaderPass::compile");

        GfxShaderLang gfx_lang = driver->shader_lang();

        const std::string& sources_vertex   = get_vertex(gfx_lang);
        const std::string& sources_fragment = get_fragment(gfx_lang);
        const std::string& sources_compute  = get_compute(gfx_lang);

        const bool has_vertex   = !sources_vertex.empty();
        const bool has_fragment = !sources_fragment.empty();
        const bool has_compute  = !sources_compute.empty();

        ShaderBuilder builder;
        builder.key = name;

        if (has_vertex) builder.configure_vs();
        if (has_fragment) builder.configure_fs();
        if (has_compute) builder.configure_cs();

        if (gfx_lang == GfxShaderLang::GlslVk450) {
            builder.add_vs_module("#version 450 core\n");
            builder.add_fs_module("#version 450 core\n");
            builder.add_cs_module("#version 450 core\n");
        }
        if (gfx_lang == GfxShaderLang::GlslGl410) {
            builder.add_vs_module("#version 410 core\n");
            builder.add_fs_module("#version 410 core\n");
            builder.add_cs_module("#version 410 core\n");
        }

        builder.add_define_vs("VERTEX_SHADER");
        builder.add_define_fs("FRAGMENT_SHADER");
        builder.add_define_cs("COMPUTE_SHADER");

        builder.add_defines(defines);

        int location_index = 0;

        if (has_vertex || has_fragment) {
            attribs.for_each([&](int i, GfxVertAttrib attrib) {
                builder.add_define("ATTRIB_" + Enum::to_str(attrib));
                builder.vertex.value() << "layout(location = " << location_index << ") in "
                                       << GfxVertAttribGlslTypes[i] << " "
                                       << "in" << Enum::to_str(attrib)
                                       << ";\n";
                location_index += 1;
            });
        }

        if (shader) {
            builder.add_vs_module(StringUtils::find_replace_first(sources_vertex, "__SHADER_CODE_VERTEX__", shader->get_include_parameters() + "\n" + shader->get_vertex()));
            builder.add_fs_module(StringUtils::find_replace_first(sources_fragment, "__SHADER_CODE_FRAGMENT__", shader->get_include_parameters() + "\n" + shader->get_include_textures() + "\n" + shader->get_fragment()));
            builder.add_cs_module(StringUtils::find_replace_first(sources_compute, "__SHADER_CODE_COMPUTE__", shader->get_include_parameters() + "\n" + shader->get_include_textures() + "\n" + shader->get_compute()));

        } else {
            builder.add_vs_module(sources_vertex);
            builder.add_fs_module(sources_fragment);
            builder.add_cs_module(sources_compute);
        }

        GfxDescSetLayoutDescs layouts_desc;
        fill_layout(layouts_desc, shader);

        for (int i = 0; i < int(layouts_desc.size()); i++) {
            builder.layouts.push_back(driver->make_desc_layout(layouts_desc[i], SID(name.str() + "@" + StringUtils::from_int(i))));
        }

        if (!builder.compile()) {
            WG_LOG_ERROR("failed to build shader " << name);
            return StatusCode::FailedCompile;
        }

        assert(builder.gfx_shader);
        out_shader = std::move(builder.gfx_shader);

        return StatusCode::Ok;
    }

}// namespace wmoge
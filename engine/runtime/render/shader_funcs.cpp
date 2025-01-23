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

#include "shader_funcs.hpp"

#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

namespace wmoge {

    void ShaderFuncs::fill(RdgGraph& graph, Strid name, RdgTexture* texture, Vec4f fill_value, ShaderTable* table) {
        WG_PROFILE_RDG_SCOPE("ShaderFuncs::fill", graph);
        graph.add_compute_pass(name)
                .storage(texture)
                .bind([fill_value, table, name, texture](RdgPassContext& context) {
                    auto fill_shader = table->fill();

                    bind_pso_compute(context, fill_shader, fill_shader->tq_default, fill_shader->tq_default.ps_default, {});

                    auto param_block = make_param_block(context, fill_shader, name);
                    param_block->set_var(fill_shader->pb_default.fillvalue, fill_value);
                    param_block->set_var(fill_shader->pb_default.result, texture->get_texture_ref());
                    context.bind_param_block(param_block);

                    const int w = texture->get_desc().width;
                    const int h = texture->get_desc().height;
                    context.dispatch(GfxUtils::group_size(w, h, ShaderFill::Constants::GROUP_SIZE_DEFAULT));

                    return WG_OK;
                });
    }

    void ShaderFuncs::blit(RdgGraph& graph, Strid name, const Ref<Window>& window, RdgTexture* source, ShaderTable* table) {
        WG_PROFILE_RDG_SCOPE("ShaderFuncs::blit", graph);
        graph.add_graphics_pass(name)
                .window_target(window)
                .sampling(source)
                .bind([table, name, source, window](RdgPassContext& context) {
                    auto blit_shader = table->blit();

                    auto param_block = make_param_block(context, blit_shader, name);
                    param_block->set_var(blit_shader->pb_default.inversegamma, 1.0f / 2.0f);
                    param_block->set_var(blit_shader->pb_default.imagetexture, source->get_texture_ref());
                    context.validate_param_block(param_block);

                    const int w = window->fbo_width();
                    const int h = window->fbo_height();

                    context.begin_render_pass();
                    context.viewport(Rect2i{0, 0, w, h});

                    bind_pso_graphics(context, blit_shader, blit_shader->tq_default, blit_shader->tq_default.ps_default, {}, {});

                    context.bind_param_block(param_block);
                    context.draw(3, 0, 1);
                    context.end_render_pass();

                    return WG_OK;
                });
    }

}// namespace wmoge
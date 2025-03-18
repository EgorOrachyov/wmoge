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

#include "rdg/rdg_profiling.hpp"

namespace wmoge {

    void ShaderFuncs::fill(RdgGraph& graph, Strid name, RdgTexture* texture, Vec4f fill_value, ShaderTable* table) {
        WG_PROFILE_RDG_SCOPE("ShaderFuncs::fill", graph);

        ShaderFill::ParamBlockDefault::Vars params;
        params.fillvalue  = fill_value;
        params.result     = texture;
        auto* param_block = create_param_block<ShaderFill::ParamBlockDefault>(graph, params, table->fill());

        const Vec2i size = {texture->get_desc().width, texture->get_desc().height};

        graph.add_compute_pass(name)
                .storage(texture)
                .params(param_block)
                .bind([table, size, param_block](RdgPassContext& context) {
                    auto fill_shader = table->fill();
                    bind_pso_compute(context, fill_shader, fill_shader->tq_default.ps_default, {});
                    bind_param_block(context, param_block);
                    context.dispatch(GfxUtils::group_size(size.x(), size.y(), ShaderFill::Constants::GROUP_SIZE_DEFAULT));
                    return WG_OK;
                });
    }

    void ShaderFuncs::blit(RdgGraph& graph, Strid name, const Ref<Window>& window, RdgTexture* source, ShaderTable* table) {
        WG_PROFILE_RDG_SCOPE("ShaderFuncs::blit", graph);

        ShaderBlit::ParamBlockDefault::Vars params;
        params.gamma                = 2.2f;
        params.inversegamma         = 1.0f / 2.2f;
        params.imagetexture         = source;
        params.imagetexture_sampler = graph.get_sampler(DefaultSampler::Default);
        auto* param_block           = create_param_block<ShaderBlit::ParamBlockDefault>(graph, params, table->blit());

        const Vec2i size = {window->fbo_width(), window->fbo_height()};

        graph.add_graphics_pass(name)
                .window_target(window)
                .sampling(source)
                .params(param_block)
                .bind([table, size, param_block](RdgPassContext& context) {
                    auto blit_shader = table->blit();
                    context.viewport(Rect2i{0, 0, size.x(), size.y()});
                    bind_pso_graphics(context, blit_shader, blit_shader->tq_default.ps_default, {blit_shader->tq_default.options.out_mode_srgb}, {});
                    bind_param_block(context, param_block);
                    context.draw(3, 0, 1);
                    return WG_OK;
                });
    }

}// namespace wmoge
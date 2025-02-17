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

#pragma once

#include "platform/window.hpp"
#include "rdg/rdg_graph.hpp"
#include "render/shader_table.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class ShaderFuncs
     * @brief Common funcs for constructing shading algorithms
     */
    class ShaderFuncs {
    public:
        template<typename ParamBlockType, typename ShaderType>
        static ParamBlockType* create_param_block(RdgGraph& graph, ShaderType* shader);

        template<typename ParamBlockType, typename ShaderType>
        static ParamBlockType* create_param_block(RdgGraph& graph, typename ParamBlockType::Vars vars, ShaderType* shader);

        template<typename ShaderType, typename PassType>
        static void bind_pso_graphics(RdgPassContext& context, ShaderType* shader, const PassType& pass, const buffered_vector<ShaderOptionVariant>& options, const GfxVertAttribs& attribs);

        template<typename ShaderType, typename PassType>
        static void bind_pso_compute(RdgPassContext& context, ShaderType* shader, const PassType& pass, const buffered_vector<ShaderOptionVariant>& options);

        template<typename ParamBlockType>
        static void bind_param_block(RdgPassContext& context, ParamBlockType* param_block);

        static void fill(RdgGraph& graph, Strid name, RdgTexture* texture, Vec4f fill_value, ShaderTable* table);

        static void blit(RdgGraph& graph, Strid name, const Ref<Window>& window, RdgTexture* source, ShaderTable* table);
    };

    template<typename ParamBlockType, typename ShaderType>
    inline ParamBlockType* ShaderFuncs::create_param_block(RdgGraph& graph, ShaderType* shader) {
        return static_cast<ParamBlockType*>(graph.create_param_block([shader](RdgResourceId id) {
            return make_ref<ParamBlockType>(shader, id);
        }));
    }

    template<typename ParamBlockType, typename ShaderType>
    inline ParamBlockType* ShaderFuncs::create_param_block(RdgGraph& graph, typename ParamBlockType::Vars vars, ShaderType* shader) {
        auto* param_block = create_param_block<ParamBlockType, ShaderType>(graph, shader);
        param_block->vars = std::move(vars);
        return param_block;
    }

    template<typename ShaderType, typename PassType>
    inline void ShaderFuncs::bind_pso_graphics(RdgPassContext& context, ShaderType* shader, const PassType& pass, const buffered_vector<ShaderOptionVariant>& options, const GfxVertAttribs& attribs) {
        context.bind_pso_graphics(shader->shader.get(), pass.technique_name, pass.pass_name, options, attribs);
    }

    template<typename ShaderType, typename PassType>
    inline void ShaderFuncs::bind_pso_compute(RdgPassContext& context, ShaderType* shader, const PassType& pass, const buffered_vector<ShaderOptionVariant>& options) {
        context.bind_pso_compute(shader->shader.get(), pass.technique_name, pass.pass_name, options);
    }

    template<typename ParamBlockType>
    inline void ShaderFuncs::bind_param_block(RdgPassContext& context, ParamBlockType* param_block) {
        context.bind_param_block(param_block->get_param_block());
    }

}// namespace wmoge
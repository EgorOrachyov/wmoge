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

#include "shader_table.hpp"

#include "asset/asset_manager.hpp"
#include "grc/shader_manager.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status ShaderTable::reflect_types(ShaderManager* shader_manager) {
        WG_PROFILE_CPU_RENDER("ShaderTable::reflect_types");

        return reflect_shader_types(shader_manager);
    }

#define LOAD_SHADER(shader_name)                                  \
    {                                                             \
        Ref<Shader> shader;                                       \
        WG_CHECKED(load_shader(#shader_name, shader));            \
        WG_CHECKED(m_##shader_name.load_from(std::move(shader))); \
    }

    Status ShaderTable::load_shaders(AssetManager* asset_manager) {
        WG_PROFILE_CPU_RENDER("ShaderTable::load_shaders");

        auto load_shader = [&](const std::string& name, Ref<Shader>& shader) -> Status {
            const std::string shader_path = "engine/shaders/" + name + ".shader";
            shader                        = asset_manager->load_wait(shader_path).cast<Shader>();

            if (!shader) {
                WG_LOG_ERROR("failed load shader " << shader_path);
                return StatusCode::NoAsset;
            }

            return WG_OK;
        };

        LOAD_SHADER(aux_draw);
        LOAD_SHADER(blit);
        LOAD_SHADER(canvas);
        LOAD_SHADER(fill);

        return WG_OK;
    }

#undef LOAD_SHADER

}// namespace wmoge
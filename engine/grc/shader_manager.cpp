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

#include "shader_manager.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "grc/shader_builder.hpp"
#include "io/enum.hpp"
#include "math/math_utils.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    ShaderManager::ShaderManager() {
        WG_AUTO_PROFILE_GRC("ShaderManager::ShaderManager");

        m_texture_manager = IocContainer::instance()->resolve_v<TextureManager>();
        m_file_system     = IocContainer::instance()->resolve_v<FileSystem>();
        m_gfx_driver      = IocContainer::instance()->resolve_v<GfxDriver>();
        m_console         = IocContainer::instance()->resolve_v<Console>();

        auto builtin_types = ShaderTypes::builtin();

        for (auto& type : builtin_types) {
            add_global_type(type);
        }

        m_shaders_folder = "root://shaders";
        m_file_system->add_rule({m_shaders_folder, "root://../shaders"});
    }

    Status ShaderManager::fit_shader(const Ref<Shader>& script) {
        WG_AUTO_PROFILE_GRC("ShaderManager::fit_shader");

        std::lock_guard guard(m_mutex);

        auto q = m_shaders.find(script->get_name());
        if (q != m_shaders.end()) {
            WG_LOG_ERROR("clash in shader name " << script->get_name());
            return StatusCode::Error;
        }

        m_shaders[script->get_name()] = script;
        return StatusCode::Ok;
    }

    Ref<Shader> ShaderManager::find_shader(Strid name) {
        std::lock_guard guard(m_mutex);

        auto q = m_shaders.find(name);
        return q != m_shaders.end() ? q->second : Ref<Shader>();
    }

    void ShaderManager::add_global_type(const Ref<ShaderType>& type) {
        std::lock_guard guard(m_mutex);

        m_global_types[type->name] = type;
    }

    std::optional<Ref<ShaderType>> ShaderManager::find_global_type(Strid name) {
        std::lock_guard guard(m_mutex);

        auto q = m_global_types.find(name);
        if (q != m_global_types.end()) {
            return q->second;
        }

        return std::optional<Ref<ShaderType>>();
    }

}// namespace wmoge
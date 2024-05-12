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
#include "debug/console.hpp"
#include "grc/shader_builder.hpp"
#include "grc/shader_library.hpp"
#include "io/enum.hpp"
#include "math/math_utils.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "rtti/type_storage.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    ShaderManager::ShaderManager() {
        WG_AUTO_PROFILE_GRC("ShaderManager::ShaderManager");

        m_file_system = IocContainer::iresolve_v<FileSystem>();
        m_gfx_driver  = IocContainer::iresolve_v<GfxDriver>();
        m_console     = IocContainer::iresolve_v<Console>();

        Config* config = IocContainer::iresolve_v<Config>();

        m_shaders_folder          = "engine://shaders";
        m_compilation_enable      = true;
        m_hot_reload_enable       = true;
        m_hot_reload_on_change    = true;
        m_hot_reload_on_trigger   = true;
        m_hot_reload_interval_sec = 5.0f;

        config->get_string(SID("grc.shader.shaders_folder"), m_shaders_folder);
        config->get_bool(SID("grc.shader.compilation_enable"), m_compilation_enable);
        config->get_bool(SID("grc.shader.hot_reload_enable"), m_hot_reload_enable);
        config->get_bool(SID("grc.shader.hot_reload_on_change"), m_hot_reload_on_change);
        config->get_bool(SID("grc.shader.hot_reload_on_trigger"), m_hot_reload_on_trigger);
        config->get_float(SID("grc.shader.hot_reload_interval_sec"), m_hot_reload_interval_sec);

        auto builtin_types = ShaderTypes::builtin();
        for (auto& type : builtin_types) {
            add_global_type(type);
        }

        m_compiler_options = ShaderCompilerOptions();// debug options

        m_compiler_env.path_includes.insert(m_shaders_folder);
    }

    std::optional<Ref<ShaderType>> ShaderManager::find_global_type(Strid name) {
        auto q = m_global_types.find(name);
        if (q != m_global_types.end()) {
            return q->second;
        }

        return std::optional<Ref<ShaderType>>();
    }

    ShaderCompiler* ShaderManager::find_compiler(GfxShaderPlatform platform) {
        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_compilers[int(platform)].get();
    }

    void ShaderManager::add_global_type(const Ref<ShaderType>& type) {
        m_global_types[type->name] = type;
    }

    void ShaderManager::load_compilers() {
        WG_AUTO_PROFILE_GRC("ShaderManager::load_compilers");

        RttiTypeStorage* type_storage = IocContainer::iresolve_v<RttiTypeStorage>();

        auto compilers = type_storage->find_classes([](const Ref<RttiClass>& rtti) {
            return rtti->is_subtype_of(ShaderCompiler::get_class_static()) && rtti->can_instantiate();
        });

        for (const auto& compiler_class : compilers) {
            Ref<ShaderCompiler> compiler = compiler_class->instantiate().cast<ShaderCompiler>();
            assert(compiler);
            assert(int(compiler->get_platform()) < int(GfxShaderPlatform::Max));
            m_compilers[int(compiler->get_platform())] = compiler;
        }
    }

}// namespace wmoge
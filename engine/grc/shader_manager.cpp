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
#include "rtti/type_storage.hpp"
#include "system/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    ShaderManager::ShaderManager() {
        WG_AUTO_PROFILE_GRC("ShaderManager::ShaderManager");

        m_file_system = IocContainer::instance()->resolve_v<FileSystem>();
        m_gfx_driver  = IocContainer::instance()->resolve_v<GfxDriver>();
        m_console     = IocContainer::instance()->resolve_v<Console>();

        auto builtin_types = ShaderTypes::builtin();

        for (auto& type : builtin_types) {
            add_global_type(type);
        }

        m_shaders_folder = "root://shaders";
        m_file_system->add_rule({m_shaders_folder, "root://../shaders"});
    }

    std::optional<Ref<ShaderType>> ShaderManager::find_global_type(Strid name) {
        auto q = m_global_types.find(name);
        if (q != m_global_types.end()) {
            return q->second;
        }

        return std::optional<Ref<ShaderType>>();
    }

    void ShaderManager::add_global_type(const Ref<ShaderType>& type) {
        m_global_types[type->name] = type;
    }

    void ShaderManager::load_compilers() {
        WG_AUTO_PROFILE_GRC("ShaderManager::load_compilers");

        RttiTypeStorage* type_storage = IocContainer::instance()->resolve_v<RttiTypeStorage>();

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
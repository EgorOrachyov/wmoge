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

#include "core/flat_map.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_reflection.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class ShaderManager
     * @brief Manager for loading and compilation of shader scripts
    */
    class ShaderManager {
    public:
        ShaderManager();

        std::optional<Ref<ShaderType>> find_global_type(Strid name);
        ShaderCompiler*                find_compiler(GfxShaderPlatform platform);
        void                           add_global_type(const Ref<ShaderType>& type);
        void                           load_compilers();

        [[nodiscard]] const std::string&           get_shaders_folder() const { return m_shaders_folder; }
        [[nodiscard]] const ShaderCompilerOptions& get_compiler_options() const { return m_compiler_options; }
        [[nodiscard]] const ShaderCompilerEnv&     get_compiler_env() const { return m_compiler_env; }

    private:
        std::array<Ref<ShaderCompiler>, GfxLimits::NUM_PLATFORMS> m_compilers;
        ShaderCompilerOptions                                     m_compiler_options;
        ShaderCompilerEnv                                         m_compiler_env;
        flat_map<Strid, Ref<ShaderType>>                          m_global_types;
        std::string                                               m_shaders_folder;

        class FileSystem* m_file_system = nullptr;
        class GfxDriver*  m_gfx_driver  = nullptr;
        class Console*    m_console     = nullptr;
    };

}// namespace wmoge
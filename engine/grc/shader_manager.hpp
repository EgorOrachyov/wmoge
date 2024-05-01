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
#include "debug/console.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/shader.hpp"
#include "grc/shader_reflection.hpp"
#include "grc/texture_manager.hpp"
#include "platform/file_system.hpp"

#include <mutex>

namespace wmoge {

    struct ShaderCompilerInput {
        ShaderPermutation permutation;
        Shader*           shader        = nullptr;
        std::int16_t      technique_idx = -1;
        std::int16_t      pass_idx      = -1;
    };

    struct ShaderCompilerOutput {
        Ref<GfxShader>                  shader;
        Ref<GfxShaderProgram>           program;
        buffered_vector<std::string, 1> errors;
    };

    /**
     * @class ShaderManager
     * @brief Manager for loading and compilation of shader scripts
    */
    class ShaderManager {
    public:
        ShaderManager();

        Status                         fit_shader(const Ref<Shader>& shader);
        Ref<Shader>                    find_shader(Strid name);
        void                           add_global_type(const Ref<ShaderType>& type);
        std::optional<Ref<ShaderType>> find_global_type(Strid name);

        [[nodiscard]] const std::string& get_shaders_folder() const { return m_shaders_folder; }

    private:
        flat_map<Strid, Ref<Shader>>     m_shaders;
        flat_map<Strid, Ref<ShaderType>> m_global_types;
        std::string                      m_shaders_folder;
        TextureManager*                  m_texture_manager;
        FileSystem*                      m_file_system;
        GfxDriver*                       m_gfx_driver;
        Console*                         m_console;

        std::recursive_mutex m_mutex;
    };

}// namespace wmoge
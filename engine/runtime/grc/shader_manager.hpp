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

#include "core/async.hpp"
#include "core/flat_map.hpp"
#include "core/synchronization.hpp"
#include "core/weak_ref.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/shader.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_reflection.hpp"
#include "platform/file_system.hpp"

#include <functional>
#include <memory>
#include <mutex>

namespace wmoge {

    /**
     * @class ShaderManager
     * @brief Global shaders manager
    */
    class ShaderManager {
    public:
        ShaderManager();

        Status                         load_shader_reflection(const ShaderFile& file, ShaderReflection& reflection);
        void                           add_shader(const Ref<Shader>& shader);
        void                           remove_shader(Shader* shader);
        bool                           has_shader(Shader* shader);
        const GfxDescSetLayoutRef&     get_shader_layout(Shader* shader, std::int16_t space);
        const GfxPsoLayoutRef&         get_shader_pso_layout(Shader* shader);
        Ref<GfxShaderProgram>          get_or_create_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation);
        Ref<GfxShaderProgram>          find_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation);
        std::optional<ShaderStatus>    find_program_status(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation);
        Async                          precache_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation);
        Async                          compile_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation, Ref<ShaderCompilerRequest>& request, Async depends_on = Async());
        Status                         load_cache(Shader* shader, GfxShaderPlatform platform, bool allow_missing = true);
        Status                         save_cache(Shader* shader, GfxShaderPlatform platform);
        std::optional<Ref<ShaderType>> find_global_type(Strid name);
        ShaderCompiler*                find_compiler(GfxShaderPlatform platform);
        void                           add_global_type(const Ref<ShaderType>& type);
        void                           load_compilers();

        [[nodiscard]] const std::string&           get_shaders_folder() const { return m_shaders_folder; }
        [[nodiscard]] const ShaderCompilerOptions& get_compiler_options() const { return m_compiler_options; }
        [[nodiscard]] const ShaderCompilerEnv&     get_compiler_env() const { return m_compiler_env; }
        [[nodiscard]] bool                         is_compilation_enabled() const { return m_compilation_enable; }
        [[nodiscard]] bool                         is_hot_reload_enabled() const { return m_hot_reload_enable; }
        [[nodiscard]] GfxShaderPlatform            get_active_platform() { return m_active_platform; }

    private:
        struct Entry : public WeakRefCnt<Entry, RefCnt> {
            WeakRef<Shader>   weak_ref;
            ShaderCache       cache;
            GfxDescSetLayouts layouts;
            GfxPsoLayoutRef   pso_layout;
            RwMutexReadPrefer mutex;
        };

        Entry* get_entry(Shader* shader);
        Entry& get_entry_ref(Shader* shader);
        Entry& add_entry(const Ref<Shader>& shader);
        void   remove_entry(Shader* shader);

    private:
        flat_map<Shader*, Ref<Entry>>                             m_shaders;
        std::array<Ref<ShaderCompiler>, GfxLimits::NUM_PLATFORMS> m_compilers;
        ShaderCompilerOptions                                     m_compiler_options;
        ShaderCompilerEnv                                         m_compiler_env;
        flat_map<Strid, Ref<ShaderType>>                          m_global_types;
        std::string                                               m_shaders_folder;
        std::string                                               m_shaders_cache_path;
        bool                                                      m_load_cache;
        bool                                                      m_save_cache;
        bool                                                      m_compilation_enable;
        bool                                                      m_hot_reload_enable;
        bool                                                      m_hot_reload_on_change;
        bool                                                      m_hot_reload_on_trigger;
        float                                                     m_hot_reload_interval_sec;
        GfxShaderPlatform                                         m_active_platform;
        std::shared_ptr<std::function<void(Shader*)>>             m_callback;

        class TaskManager*    m_task_manager    = nullptr;
        class FileSystem*     m_file_system     = nullptr;
        class GfxDriver*      m_gfx_driver      = nullptr;
        class TextureManager* m_texture_manager = nullptr;
        class ShaderLibrary*  m_shader_library  = nullptr;
        class PsoCache*       m_pso_cache       = nullptr;
        class Console*        m_console         = nullptr;

        mutable RwMutexReadPrefer m_mutex;
    };

}// namespace wmoge
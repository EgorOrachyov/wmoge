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

#ifndef WMOGE_SHADER_MANAGER_HPP
#define WMOGE_SHADER_MANAGER_HPP

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_shader.hpp"
#include "io/archive.hpp"

#include <filesystem>
#include <mutex>
#include <string>

namespace wmoge {

    /**
     * @class ShaderManager
     * @brief Render module access to engine shaders
     */
    class ShaderManager {
    public:
        ShaderManager();
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager(ShaderManager&&)      = delete;
        ~ShaderManager();

        StringId       make_shader_key(const StringId& shader_name, const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines, class Shader* shader);
        Ref<GfxShader> get_shader(const StringId& shader_name);
        Ref<GfxShader> get_shader(const StringId& shader_name, const fast_vector<std::string>& defines);
        Ref<GfxShader> get_shader(const StringId& shader_name, const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines);
        Ref<GfxShader> get_shader(const StringId& shader_name, const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines, class Shader* shader);
        Ref<GfxShader> find(const StringId& shader_key);
        void           cache(const StringId& shader_key, const Ref<GfxShader>& shader, bool allow_overwrite = false);
        void           dump_stats();
        void           reload_shaders();
        void           clear_cache();
        void           save_cache(const std::string& path_on_disk);
        void           load_cache(const std::string& path_on_disk);

        /**
         * @class ShaderData
         * @brief Entry holding data of a particular shader
         */
        struct ShaderData {
            StringId       name;
            Ref<GfxShader> shader;
            Ref<Data>      bytecode;

            friend bool archive_write(Archive& archive, const ShaderData& shader_data);
            friend bool archive_read(Archive& archive, ShaderData& shader_data);
        };

        /**
         * @class ShaderSources
         * @brief Shaders sources for compilation
         */
        struct ShaderSources {
            std::string modules[3];
        };

    private:
        void load_sources_from_build();
        void load_sources_from_disk();

    private:
        fast_map<StringId, ShaderData>    m_cache;
        fast_map<StringId, ShaderSources> m_sources;
        std::string                       m_shaders_directory;

        class ConsoleVar* m_var_allow_dump   = nullptr;
        class ConsoleVar* m_var_allow_reload = nullptr;
        class ConsoleCmd* m_cmd_clear        = nullptr;
        class ConsoleCmd* m_cmd_save         = nullptr;
        class ConsoleCmd* m_cmd_info         = nullptr;
        class ConsoleCmd* m_cmd_dump         = nullptr;
        class ConsoleCmd* m_cmd_reload       = nullptr;

        mutable std::recursive_mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SHADER_MANAGER_HPP

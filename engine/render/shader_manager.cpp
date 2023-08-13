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

#include "core/engine.hpp"
#include "core/string_utils.hpp"
#include "debug/console.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/archive_file.hpp"
#include "io/enum.hpp"
#include "platform/file_system.hpp"
#include "render/shader_builder.hpp"
#include "resource/shader.hpp"

#include <array>
#include <sstream>
#include <string>

// built-in opengl shaders
#include "shaders/generated/auto_aux_draw_canvas_gl410_frag.hpp"
#include "shaders/generated/auto_aux_draw_canvas_gl410_vert.hpp"
#include "shaders/generated/auto_base_gl410_frag.hpp"
#include "shaders/generated/auto_base_gl410_vert.hpp"
#include "shaders/generated/auto_text_gl410_frag.hpp"
#include "shaders/generated/auto_text_gl410_vert.hpp"

// built-in vulkan shaders
#include "shaders/generated/auto_aux_draw_canvas_vk450_frag.hpp"
#include "shaders/generated/auto_aux_draw_canvas_vk450_vert.hpp"
#include "shaders/generated/auto_base_vk450_frag.hpp"
#include "shaders/generated/auto_base_vk450_vert.hpp"
#include "shaders/generated/auto_text_vk450_frag.hpp"
#include "shaders/generated/auto_text_vk450_vert.hpp"

namespace wmoge {

    ShaderManager::ShaderManager() {
        auto* engine     = Engine::instance();
        auto* console    = engine->console();
        auto* gfx_driver = engine->gfx_driver();

        m_shaders_directory = "root://shaders";

        m_var_allow_dump   = console->register_var(SID("shaders.allow_dump"), 1, "All dump of all compiled shaders stats");
        m_var_allow_reload = console->register_var(SID("shaders.allow_reload"), 1, "All hot-reload_shaders of all engine shaders from running project directory");

        m_cmd_clear = console->register_cmd(SID("shaders.clear"), "Remove all shaders from a cache",
                                            [=](const auto&) {
                                                clear_cache();
                                                return 0;
                                            });

        m_cmd_save = console->register_cmd(SID("shaders.save"), "Save cached shaders to a disk",
                                           [=](const auto& args) {
                                               if (args.size() < 2) {
                                                   console->add_error("not enough args");
                                                   return 1;
                                               }
                                               save_cache(args[1]);
                                               return 0;
                                           });

        m_cmd_info = console->register_cmd(SID("shaders.info"), "Show cache info",
                                           [=](const auto& args) {
                                               std::lock_guard lock(m_mutex);

                                               auto total_entries = m_cache.size();
                                               auto total_size    = std::size_t(0);

                                               for (const auto& entry : m_cache) {
                                                   const ShaderData& data = entry.second;

                                                   assert(data.shader || data.bytecode);

                                                   Ref<Data> bytecode = data.bytecode ? data.bytecode : data.shader->byte_code();

                                                   if (bytecode) {
                                                       total_size += bytecode->size();
                                                   }
                                               }

                                               console->add_info("entries " + std::to_string(total_entries) + " size " + StringUtils::from_mem_size(total_size));
                                               return 0;
                                           });

        m_cmd_dump = console->register_cmd(SID("shaders.dump"), "Dump compiled shaders info and sources",
                                           [this](auto&) {
                                               dump_stats();
                                               return 0;
                                           });

        m_cmd_reload = console->register_cmd(SID("shaders.reload"), "Hot-reload_shaders shaders from running project directory (from disk)",
                                             [this](auto&) {
                                                 reload_shaders();
                                                 return 0;
                                             });

        load_sources_from_build();
        load_sources_from_disk();

        load_cache(gfx_driver->shader_cache_path());
    }
    ShaderManager::~ShaderManager() {
        auto* engine     = Engine::instance();
        auto* gfx_driver = engine->gfx_driver();

        save_cache(gfx_driver->shader_cache_path());
    }

    StringId ShaderManager::make_shader_key(const StringId& shader_name, const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines, class Shader* shader) {
        std::stringstream shader_key_builder;
        shader_key_builder << "type=" << shader_name.str();

        if (!defines.empty()) {
            shader_key_builder << " defs=[";

            for (const auto& define : defines) {
                shader_key_builder << define << ", ";
            }

            shader_key_builder << "]";
        }

        for (const GfxVertAttribs& attribs : streams) {
            if (attribs.bits.any()) {
                shader_key_builder << " [";

                for (int i = 0; i < int(GfxVertAttrib::None); i++) {
                    auto attrib = static_cast<GfxVertAttrib>(i);

                    if (attribs.get(attrib)) {
                        shader_key_builder << magic_enum::enum_name(attrib) << ", ";
                    }
                }

                shader_key_builder << "]";
            }
        }

        if (shader) {
            shader_key_builder << " shd=" << shader->get_name();
        }

        return SID(shader_key_builder.str());
    }
    Ref<GfxShader> ShaderManager::get_shader(const StringId& shader_name) {
        return get_shader(shader_name, {});
    }
    Ref<GfxShader> ShaderManager::get_shader(const StringId& shader_name, const fast_vector<std::string>& defines) {
        return get_shader(shader_name, {}, defines, nullptr);
    }
    Ref<GfxShader> ShaderManager::get_shader(const wmoge::StringId& shader_name, const wmoge::GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines) {
        return get_shader(shader_name, streams, defines, nullptr);
    }
    Ref<GfxShader> ShaderManager::get_shader(const StringId& shader_name, const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines, class Shader* shader) {
        const StringId shader_key = make_shader_key(shader_name, streams, defines, shader);
        Ref<GfxShader> gfx_shader = find(shader_key);

        if (gfx_shader) {
            return gfx_shader;
        }
        if (m_sources.find(shader_name) == m_sources.end()) {
            WG_LOG_ERROR("no such shader type to build " << shader_name);
            return gfx_shader;
        }

        Engine*       engine     = Engine::instance();
        GfxDriver*    gfx_driver = engine->gfx_driver();
        GfxShaderLang gfx_lang   = gfx_driver->shader_lang();

        ShaderBuilder builder;
        builder.configure_vs();
        builder.configure_fs();

        if (gfx_lang == GfxShaderLang::GlslVk450) {
            builder.add_vs_module("#version 450 core\n");
            builder.add_fs_module("#version 450 core\n");
            builder.add_cs_module("#version 450 core\n");
        }
        if (gfx_lang == GfxShaderLang::GlslGl410) {
            builder.add_vs_module("#version 410 core\n");
            builder.add_fs_module("#version 410 core\n");
            builder.add_cs_module("#version 410 core\n");
        }

        builder.add_defines(defines);

        int location_index = 0;

        for (const GfxVertAttribs& attribs : streams) {
            attribs.for_each([&](int i, GfxVertAttrib attrib) {
                builder.add_define("ATTRIB_" + Enum::to_str(attrib));
                builder.vertex.value() << "layout(location = " << location_index << ") in "
                                       << GfxVertAttribGlslTypes[i] << " "
                                       << "in" << Enum::to_str(attrib)
                                       << ";\n";
                location_index += 1;
            });
        }

        if (shader) {
            builder.add_vs_module(shader->get_include_parameters());
            builder.add_fs_module(shader->get_include_parameters());
            builder.add_fs_module(shader->get_include_textures());
            builder.add_vs_module(shader->get_vertex());
            builder.add_fs_module(shader->get_fragment());
            builder.add_cs_module(shader->get_compute());
        }

        const ShaderSources& sources = m_sources[shader_name];
        builder.add_vs_module(sources.modules[int(GfxShaderModule::Vertex)]);
        builder.add_fs_module(sources.modules[int(GfxShaderModule::Fragment)]);
        builder.add_fs_module(sources.modules[int(GfxShaderModule::Compute)]);

        if (!builder.compile()) {
            WG_LOG_ERROR("failed to build shader with key " << shader_key);
            return gfx_shader;
        }

        gfx_shader = std::move(builder.gfx_shader);

        cache(shader_key, gfx_shader, true);

        return gfx_shader;
    }

    Ref<GfxShader> ShaderManager::find(const StringId& shader_key) {
        std::lock_guard lock(m_mutex);

        auto query = m_cache.find(shader_key);
        if (query == m_cache.end()) {
            return nullptr;
        }

        auto& entry = query->second;

        if (entry.shader) {
            return entry.shader;
        }
        if (entry.bytecode) {
            GfxDriver* gfx_driver = Engine::instance()->gfx_driver();
            entry.shader          = gfx_driver->make_shader(entry.bytecode, entry.name);
            return entry.shader;
        }

        assert(false);

        return nullptr;
    }
    void ShaderManager::cache(const StringId& shader_key, const Ref<GfxShader>& shader, bool allow_overwrite) {
        WG_AUTO_PROFILE_RENDER("ShaderManager::cache");

        std::lock_guard lock(m_mutex);

        auto& entry = m_cache[shader_key];
        if (entry.shader && !allow_overwrite) {
            WG_LOG_WARNING("attempt to cache already cached shader");
            return;
        }

        entry.shader = shader;
        entry.name   = shader->name();
        WG_LOG_INFO("cache new shader " << shader_key);
    }
    void ShaderManager::dump_stats() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::dump_stats");

        if (m_var_allow_dump->as_int()) {
            std::lock_guard lock(m_mutex);

            const int total_entries = int(m_cache.size());
            int       current_entry = 1;

            for (const auto& entry : m_cache) {
                const StringId&   key  = entry.first;
                const ShaderData& data = entry.second;

                Ref<Data> bytecode = data.bytecode ? data.bytecode : data.shader->byte_code();

                WG_LOG_INFO("[" << current_entry << "/" << total_entries << "] '" << key << "' size "
                                << (bytecode ? StringUtils::from_mem_size(bytecode->size()) : std::string("<none>")));

                current_entry += 1;
            }
        }
    }
    void ShaderManager::reload_shaders() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::reload_shaders");

        if (m_var_allow_reload->as_int()) {
            clear_cache();
            load_sources_from_disk();
        }
    }
    void ShaderManager::clear_cache() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::clear_cache");

        std::lock_guard lock(m_mutex);
        m_cache.clear();

        WG_LOG_INFO("clear shader cache");
    }
    void ShaderManager::save_cache(const std::string& path_on_disk) {
        WG_AUTO_PROFILE_RENDER("ShaderManager::save_cache");

        std::lock_guard lock(m_mutex);

        Engine*     engine      = Engine::instance();
        FileSystem* file_system = engine->file_system();

        std::fstream file;
        if (!file_system->open_file(path_on_disk, file, std::ios_base::out | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to open shader cache file to save " << path_on_disk);
            return;
        }

        for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
            if (!it->second.bytecode) {
                Ref<Data> bytecode = it->second.shader->byte_code();
                if (!bytecode) {
                    it = m_cache.erase(it);
                    continue;
                }
                it->second.bytecode = std::move(bytecode);
            }
        }

        ArchiveWriterFile archive(file);
        archive << m_cache;

        WG_LOG_INFO("save shader cache: " << path_on_disk << " " << StringUtils::from_mem_size(archive.get_size()));
    }
    void ShaderManager::load_cache(const std::string& path_on_disk) {
        WG_AUTO_PROFILE_RENDER("ShaderManager::load");

        std::lock_guard lock(m_mutex);

        Engine*     engine      = Engine::instance();
        FileSystem* file_system = engine->file_system();

        if (!file_system->exists(path_on_disk)) {
            WG_LOG_INFO("no cache to load");
            return;
        }

        std::fstream file;
        if (!file_system->open_file(path_on_disk, file, std::ios_base::in | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to open shader cache file to load " << path_on_disk);
            return;
        }

        ArchiveReaderFile archive(file);
        archive >> m_cache;

        WG_LOG_INFO("load shader cache: " << path_on_disk << " " << StringUtils::from_mem_size(archive.get_size()));
    }

    Status archive_write(Archive& archive, const ShaderManager::ShaderData& shader_data) {
        archive << shader_data.name;
        archive << shader_data.bytecode;
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, ShaderManager::ShaderData& shader_data) {
        archive >> shader_data.name;
        archive >> shader_data.bytecode;
        return StatusCode::Ok;
    }

    void ShaderManager::load_sources_from_build() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::load_sources_from_build");

        GfxDriver*    gfx_driver = Engine::instance()->gfx_driver();
        GfxShaderLang gfx_lang   = gfx_driver->shader_lang();

        const std::pair<const char*, const char*> sources_vk[] = {
                {source_aux_draw_canvas_vk450_vert, source_aux_draw_canvas_vk450_frag},
                {source_base_vk450_vert, source_base_vk450_frag},
                {source_text_vk450_vert, source_text_vk450_frag}};

        const std::pair<const char*, const char*> sources_gl[] = {
                {source_aux_draw_canvas_gl410_vert, source_aux_draw_canvas_gl410_frag},
                {source_base_gl410_vert, source_base_gl410_frag},
                {source_text_gl410_vert, source_text_gl410_frag}};

        const StringId names[] = {
                SID("aux_draw_canvas"),
                SID("base"),
                SID("text")};

        const int num_shaders = int(std::size(names));

        auto fill_sources = [&](const std::pair<const char*, const char*> sources[]) {
            for (int i = 0; i < num_shaders; i++) {
                auto& shader_sources      = m_sources[names[i]];
                shader_sources.modules[0] = sources[i].first;
                shader_sources.modules[1] = sources[i].second;
            }
        };

        if (gfx_lang == GfxShaderLang::GlslVk450) {
            fill_sources(sources_vk);
            WG_LOG_INFO("load built-in sources GlslVk450");
        }
        if (gfx_lang == GfxShaderLang::GlslGl410) {
            fill_sources(sources_gl);
            WG_LOG_INFO("load built-in sources GlslGl410");
        }
    }
    void ShaderManager::load_sources_from_disk() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::load_sources_from_disk");

        Engine*       engine      = Engine::instance();
        FileSystem*   file_system = engine->file_system();
        GfxDriver*    gfx_driver  = engine->gfx_driver();
        GfxShaderLang gfx_lang    = gfx_driver->shader_lang();

        std::string s_lang;

        if (gfx_lang == GfxShaderLang::GlslVk450) {
            s_lang = "vk450";
        }
        if (gfx_lang == GfxShaderLang::GlslGl410) {
            s_lang = "gl410";
        }

        auto make_module_path = [&](const StringId& name, const char* module) {
            std::stringstream key;
            key << "root://shaders/auto_" << name.str() << "_" << s_lang << "_" << module << ".glsl";
            return key.str();
        };

        for (auto& entry : m_sources) {
            const StringId& shader_name = entry.first;
            ShaderSources&  sources     = entry.second;

            std::string vertex_module_path = make_module_path(shader_name, "vert");
            if (file_system->read_file(vertex_module_path, sources.modules[0])) {
                WG_LOG_INFO("re-load from disk " << vertex_module_path);
            }

            std::string fragment_module_path = make_module_path(shader_name, "frag");
            if (file_system->read_file(fragment_module_path, sources.modules[1])) {
                WG_LOG_INFO("re-load from disk " << fragment_module_path);
            }
        }
    }

}// namespace wmoge

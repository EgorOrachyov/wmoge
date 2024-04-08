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

#include "core/string_utils.hpp"
#include "debug/console.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/archive_file.hpp"
#include "io/enum.hpp"
#include "platform/file_system.hpp"
#include "render/shader_builder.hpp"
#include "resource/config_file.hpp"
#include "resource/shader.hpp"
#include "system/engine.hpp"

// built-in passes
#include "shaders/generated/auto_base_pass.hpp"
#include "shaders/generated/auto_bloom_pass.hpp"
#include "shaders/generated/auto_canvas_pass.hpp"
#include "shaders/generated/auto_composition_pass.hpp"
#include "shaders/generated/auto_luminance_avg_pass.hpp"
#include "shaders/generated/auto_luminance_histogram_pass.hpp"
#include "shaders/generated/auto_material_pass.hpp"
#include "shaders/generated/auto_text_pass.hpp"
#include "shaders/generated/auto_tonemap_pass.hpp"

#include <array>
#include <sstream>
#include <string>

namespace wmoge {

    Status archive_write(Archive& archive, const ShaderData& shader_data) {
        archive << shader_data.name;
        archive << shader_data.bytecode;
        return StatusCode::Ok;
    }

    Status archive_read(Archive& archive, ShaderData& shader_data) {
        archive >> shader_data.name;
        archive >> shader_data.bytecode;
        return StatusCode::Ok;
    }

    ShaderManager::ShaderManager() {
        Engine*     engine  = Engine::instance();
        Console*    console = engine->console();
        ConfigFile* config  = engine->config();

        m_save_cache = config->get_bool(SID("render.shader_manager.save_cache"), false);

        m_file_system = engine->file_system();
        m_driver      = engine->gfx_driver();

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

        load_cache(m_driver->shader_cache_path());

        WG_LOG_INFO("init shader manager");
    }
    ShaderManager::~ShaderManager() {
        if (m_save_cache) {
            // save cache on exit, can disable it by config
            save_cache(m_driver->shader_cache_path());
        }
    }

    Strid ShaderManager::make_shader_key(const Strid& shader_name, const GfxVertAttribs& attribs, const buffered_vector<std::string>& defines, class Shader* shader) {
        std::stringstream shader_key_builder;
        shader_key_builder << "type=" << shader_name.str();

        if (!defines.empty()) {
            shader_key_builder << " defs=[";

            for (const auto& define : defines) {
                shader_key_builder << define << ", ";
            }

            shader_key_builder << "]";
        }

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

        if (shader) {
            shader_key_builder << " shd=" << shader->get_name();
        }

        return SID(shader_key_builder.str());
    }
    Ref<GfxShader> ShaderManager::get_shader(const Strid& shader_name) {
        return get_shader(shader_name, {});
    }
    Ref<GfxShader> ShaderManager::get_shader(const Strid& shader_name, const buffered_vector<std::string>& defines) {
        return get_shader(shader_name, {}, defines, nullptr);
    }
    Ref<GfxShader> ShaderManager::get_shader(const wmoge::Strid& shader_name, const GfxVertAttribs& attribs, const buffered_vector<std::string>& defines) {
        return get_shader(shader_name, attribs, defines, nullptr);
    }
    Ref<GfxShader> ShaderManager::get_shader(const Strid& shader_name, const GfxVertAttribs& attribs, const buffered_vector<std::string>& defines, class Shader* shader) {
        const Strid    shader_key = make_shader_key(shader_name, attribs, defines, shader);
        Ref<GfxShader> gfx_shader = find(shader_key);

        if (gfx_shader) {
            return gfx_shader;
        }

        ShaderPass* pass;
        {
            std::lock_guard lock(m_mutex);

            auto iter = m_passes.find(shader_name);
            if (iter == m_passes.end()) {
                WG_LOG_ERROR("no such shader type to build " << shader_name);
                return gfx_shader;
            }
            pass = iter->second.get();
        }

        if (!pass->compile(shader_key, m_driver, attribs, defines, shader, gfx_shader)) {
            WG_LOG_ERROR("failed compilation of pass " << pass->get_name());
            return gfx_shader;
        }

        cache(shader_key, gfx_shader, true);

        return gfx_shader;
    }

    Ref<GfxShader> ShaderManager::find(const Strid& shader_key) {
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
            entry.shader = m_driver->make_shader(entry.bytecode, entry.name);
            return entry.shader;
        }

        assert(false);

        return nullptr;
    }
    void ShaderManager::cache(const Strid& shader_key, const Ref<GfxShader>& shader, bool allow_overwrite) {
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
                const Strid&      key  = entry.first;
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

        std::fstream file;
        if (!m_file_system->open_file_physical(path_on_disk, file, std::ios_base::out | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to open shader cache file to save " << path_on_disk);
            return;
        }

        auto it     = m_cache.begin();
        auto it_end = m_cache.end();

        while (it != it_end) {
            if (!it->second.bytecode) {
                Ref<Data> bytecode = it->second.shader->byte_code();
                if (!bytecode) {
                    it = m_cache.erase(it);
                    continue;
                }
                it->second.bytecode = std::move(bytecode);
            }
            ++it;
        }

        ArchiveWriterFile archive(file);
        archive << m_cache;

        WG_LOG_INFO("save shader cache: " << path_on_disk << " " << StringUtils::from_mem_size(archive.get_size()));
    }
    void ShaderManager::load_cache(const std::string& path_on_disk) {
        WG_AUTO_PROFILE_RENDER("ShaderManager::load");

        std::lock_guard lock(m_mutex);

        std::fstream file;
        if (!m_file_system->open_file_physical(path_on_disk, file, std::ios_base::in | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to open shader cache file to load " << path_on_disk);
            return;
        }

        ArchiveReaderFile archive(file);
        archive >> m_cache;

        WG_LOG_INFO("load shader cache: " << path_on_disk << " " << StringUtils::from_mem_size(archive.get_size()));
    }
    void ShaderManager::register_pass(std::unique_ptr<ShaderPass> pass) {
        WG_AUTO_PROFILE_RENDER("ShaderManager::register_pass");

        std::lock_guard lock(m_mutex);

        m_passes[pass->get_name()] = std::move(pass);
    }

    void ShaderManager::load_sources_from_build() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::load_sources_from_build");

        // register engine shader passes here
        register_pass(std::make_unique<ShaderPassBase>());
        register_pass(std::make_unique<ShaderPassText>());
        register_pass(std::make_unique<ShaderPassCanvas>());
        register_pass(std::make_unique<ShaderPassMaterial>());
        register_pass(std::make_unique<ShaderPassBloom>());
        register_pass(std::make_unique<ShaderPassLuminanceAvg>());
        register_pass(std::make_unique<ShaderPassLuminanceHistogram>());
        register_pass(std::make_unique<ShaderPassTonemap>());
        register_pass(std::make_unique<ShaderPassComposition>());
    }
    void ShaderManager::load_sources_from_disk() {
        WG_AUTO_PROFILE_RENDER("ShaderManager::load_sources_from_disk");

        std::lock_guard lock(m_mutex);

        for (auto& entry : m_passes) {
            if (!entry.second->reload_sources(m_shaders_directory, m_file_system)) {
                WG_LOG_ERROR("failed to reload sources for a pass " << entry.first);
                continue;
            }
        }
    }

}// namespace wmoge

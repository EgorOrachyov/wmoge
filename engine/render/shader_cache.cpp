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

#include "shader_cache.hpp"

#include "core/engine.hpp"
#include "core/string_utils.hpp"
#include "debug/console.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/archive_file.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    ShaderCache::ShaderCache() {
        load(Engine::instance()->gfx_driver()->shader_cache_path());

        auto* console = Engine::instance()->console();

        console->register_cmd(SID("shader.cache.clear"), "Remove all shaders from a cache", [=](const auto&) {
            clear();
            return 0;
        });
        console->register_cmd(SID("shader.cache.save"), "Save cached shaders to a disk", [=](const auto& args) {
            if (args.size() < 2) {
                console->add_error("not enough args");
                return 1;
            }
            save(args[1]);
            return 0;
        });
        console->register_cmd(SID("shader.cache.info"), "Show cache info", [=](const auto& args) {
            std::lock_guard lock(m_mutex);

            auto total_entries = m_entries.size();
            auto total_size    = std::size_t(0);

            for (const auto& entry : m_entries) {
                const ShaderData& data     = entry.second;
                ref_ptr<Data>     bytecode = data.bytecode ? data.bytecode : (data.shader ? data.shader->byte_code() : nullptr);

                if (bytecode) {
                    total_size += bytecode->size();
                }
            }

            console->add_info("entries " + std::to_string(total_entries) + " size " + StringUtils::from_mem_size(total_size));
            return 0;
        });
    }
    ShaderCache::~ShaderCache() {
        save(Engine::instance()->gfx_driver()->shader_cache_path());
    }
    ref_ptr<GfxShader> ShaderCache::find(const std::string& key) {
        std::lock_guard lock(m_mutex);

        auto query = m_entries.find(key);
        if (query == m_entries.end()) {
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
    void ShaderCache::cache(const std::string& key, ref_ptr<wmoge::GfxShader> shader) {
        std::lock_guard lock(m_mutex);

        auto& entry = m_entries[key];
        if (entry.shader) {
            WG_LOG_WARNING("attempt to cache already cached shader");
            return;
        }

        entry.shader = shader;
        entry.name   = shader->name();
    }
    void ShaderCache::clear() {
        WG_AUTO_PROFILE_RENDER();

        std::lock_guard lock(m_mutex);
        m_entries.clear();

        WG_LOG_INFO("clear shader cache");
    }
    void ShaderCache::save(const std::string& path) {
        WG_AUTO_PROFILE_RENDER();

        std::lock_guard lock(m_mutex);

        Engine*     engine      = Engine::instance();
        FileSystem* file_system = engine->file_system();

        std::fstream file;
        if (!file_system->open_file(path, file, std::ios_base::out | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to open shader cache file to save " << path);
            return;
        }

        for (auto it = m_entries.begin(); it != m_entries.end(); ++it) {
            if (!it->second.bytecode) {
                ref_ptr<Data> bytecode = it->second.shader->byte_code();
                if (!bytecode) {
                    it = m_entries.erase(it);
                    continue;
                }
                it->second.bytecode = std::move(bytecode);
            }
        }

        ArchiveWriterFile archive(file);
        archive << m_entries;

        WG_LOG_INFO("save shader cache: " << path << " " << StringUtils::from_mem_size(archive.get_size()));
    }
    void ShaderCache::load(const std::string& path) {
        WG_AUTO_PROFILE_RENDER();

        std::lock_guard lock(m_mutex);

        Engine*     engine      = Engine::instance();
        FileSystem* file_system = engine->file_system();

        if (!file_system->exists(path)) {
            WG_LOG_INFO("no cache to load");
            return;
        }

        std::fstream file;
        if (!file_system->open_file(path, file, std::ios_base::in | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to open shader cache file to load " << path);
            return;
        }

        ArchiveReaderFile archive(file);
        archive >> m_entries;

        WG_LOG_INFO("load shader cache: " << path << " " << StringUtils::from_mem_size(archive.get_size()));
    }

    Archive& operator<<(Archive& archive, const ShaderCache::ShaderData& shader_data) {
        archive << shader_data.name;
        archive << shader_data.bytecode;
        return archive;
    }
    Archive& operator>>(Archive& archive, ShaderCache::ShaderData& shader_data) {
        archive >> shader_data.name;
        archive >> shader_data.bytecode;
        return archive;
    }

}// namespace wmoge
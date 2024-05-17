#include "shader_library.hpp"
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

#include "shader_library.hpp"

#include "core/date_time.hpp"
#include "core/log.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/archive_file.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "rtti/traits.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <vector>

namespace wmoge {

    struct FileShaderModule {
        WG_RTTI_STRUCT(FileShaderModule);

        Ref<Data>       bytecode;
        GfxShaderModule module_type;
        Sha256          source_hash;
        Sha256          bytecode_hash;
        Strid           name;
    };

    WG_RTTI_STRUCT_BEGIN(FileShaderModule) {
        WG_RTTI_FIELD(bytecode, {});
        WG_RTTI_FIELD(module_type, {});
        WG_RTTI_FIELD(source_hash, {});
        WG_RTTI_FIELD(bytecode_hash, {});
        WG_RTTI_FIELD(name, {});
    }
    WG_RTTI_END;

    struct FileShaderLibrary {
        WG_RTTI_STRUCT(FileShaderLibrary);

        GfxShaderPlatform             platform;
        DateTime                      timestamp;
        std::size_t                   total_size;
        std::vector<FileShaderModule> modules;
    };

    WG_RTTI_STRUCT_BEGIN(FileShaderLibrary) {
        WG_RTTI_FIELD(platform, {});
        WG_RTTI_FIELD(timestamp, {});
        WG_RTTI_FIELD(total_size, {});
        WG_RTTI_FIELD(modules, {});
    }
    WG_RTTI_END;

    ShaderModuleMap::ShaderModuleMap() {
        m_driver = IocContainer::iresolve_v<GfxDriver>();
    }

    Ref<GfxShader> ShaderModuleMap::get_or_create_shader(GfxShaderModule module_type, const Sha256& bytecode_hash) {
        auto query = m_modules.find(bytecode_hash);
        if (query == m_modules.end()) {
            return Ref<GfxShader>();
        }

        ShaderModule& shader_module = query->second;

        if (shader_module.bytecode_hash != bytecode_hash) {
            WG_LOG_ERROR("Hash mismatched for cached module for " << shader_module.name);
            return Ref<GfxShader>();
        }

        if (shader_module.module_type != module_type) {
            WG_LOG_ERROR("Hash mismatched module type for " << shader_module.name);
            return Ref<GfxShader>();
        }

        if (!shader_module.shader) {
            GfxShaderDesc shader_desc{};
            shader_desc.bytecode    = shader_module.bytecode;
            shader_desc.module_type = shader_module.module_type;
            shader_desc.shader_hash = shader_module.bytecode_hash;
            shader_module.shader    = m_driver->make_shader(std::move(shader_desc), shader_module.name);
        }

        if (!shader_module.shader) {
            WG_LOG_ERROR("Failed to create shader module for " << shader_module.name);
            return Ref<GfxShader>();
        }

        return shader_module.shader;
    }

    std::optional<Ref<GfxShader>> ShaderModuleMap::find_shader(GfxShaderModule module_type, const Sha256& bytecode_hash) {
        auto query = m_modules.find(bytecode_hash);
        if (query == m_modules.end()) {
            return std::nullopt;
        }
        if (!query->second.shader) {
            return std::nullopt;
        }
        return query->second.shader;
    }

    void ShaderModuleMap::fit_module(ShaderModule& module) {
        ShaderModule& new_entry = m_modules[module.bytecode_hash];
        new_entry               = std::move(module);
    }

    void ShaderModuleMap::dump_modules(std::vector<ShaderModule>& out_modules) {
        for (const auto& entry : m_modules) {
            out_modules.push_back(entry.second);
        }
    }

    ShaderLibrary::ShaderLibrary() {
        rtti_type<FileShaderModule>();
        rtti_type<FileShaderLibrary>();

        m_active_platform = IocContainer::iresolve_v<GfxDriver>()->get_shader_platform();

        m_library_path   = "cache://";
        m_library_prefix = "shader_library";
        m_library_suffix = "slf";
        m_load_cache     = true;
        m_save_cache     = true;

        Config* config = IocContainer::iresolve_v<Config>();
        config->get_string(SID("grc.shader.library.path"), m_library_path);
        config->get_string(SID("grc.shader.library.prefix"), m_library_prefix);
        config->get_string(SID("grc.shader.library.suffix"), m_library_suffix);
        config->get_bool(SID("grc.shader.library.load_cache"), m_load_cache);
        config->get_bool(SID("grc.shader.library.save_cache"), m_save_cache);

        if (m_load_cache) {
            load_cache(m_library_path, m_active_platform);
        }
    }

    ShaderLibrary::~ShaderLibrary() {
        if (m_save_cache) {
            save_cache(m_library_path, m_active_platform);
        }
    }

    Ref<GfxShader> ShaderLibrary::get_or_create_shader(GfxShaderPlatform platform, GfxShaderModule module_type, const Sha256& bytecode_hash) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::get_or_create_shader");

        auto query = find_shader(platform, module_type, bytecode_hash);
        if (query) {
            assert(query.value());
            return query.value();
        }

        std::unique_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_libraries[int(platform)].get_or_create_shader(module_type, bytecode_hash);
    }

    std::optional<Ref<GfxShader>> ShaderLibrary::find_shader(GfxShaderPlatform platform, GfxShaderModule module_type, const Sha256& bytecode_hash) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::find_shader");

        std::shared_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_libraries[int(platform)].find_shader(module_type, bytecode_hash);
    }

    void ShaderLibrary::fit_module(GfxShaderPlatform platform, ShaderModule& module) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::fit_module");

        std::unique_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        m_libraries[int(platform)].fit_module(module);
    }

    void ShaderLibrary::dump_modules(GfxShaderPlatform platform, std::vector<ShaderModule>& out_modules) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::dump_modules");

        std::shared_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        m_libraries[int(platform)].dump_modules(out_modules);
    }

    std::string ShaderLibrary::make_cache_file_name(const std::string& folder, const GfxShaderPlatform platform) {
        std::stringstream file_name;
        file_name << folder;
        file_name << m_library_prefix << ".";
        file_name << GfxShaderPlatformFileName[int(platform)] << ".";
        file_name << m_library_suffix;
        return file_name.str();
    }

    Status ShaderLibrary::load_cache(const std::string& folder, const GfxShaderPlatform platform) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::load_cache");

        const std::string file_path = make_cache_file_name(folder, platform);
        ArchiveFile       archive;
        IoContext         context;

        if (!archive.open(file_path, {FileOpenMode::In, FileOpenMode::Binary})) {
            WG_LOG_ERROR("failed to open shader library " << file_path << " for platform " << Enum::to_str(platform));
            return StatusCode::FailedOpenFile;
        }

        FileShaderLibrary library;
        WG_CHECKED(archive.begin_compressed());
        WG_ARCHIVE_READ(context, archive, library);
        WG_CHECKED(archive.end_compressed());

        std::unique_lock lock(m_mutex);

        if (library.platform != platform) {
            WG_LOG_ERROR("mismatched platfrom in file " << file_path);
            return StatusCode::InvalidState;
        }

        for (FileShaderModule& file_module : library.modules) {
            ShaderModule module;
            module.bytecode      = std::move(file_module.bytecode);
            module.bytecode_hash = std::move(file_module.bytecode_hash);
            module.source_hash   = std::move(file_module.source_hash);
            module.module_type   = std::move(file_module.module_type);
            module.name          = std::move(file_module.name);
            m_libraries[int(platform)].fit_module(module);
        }

        WG_LOG_INFO("load " << file_path
                            << " created=" << library.timestamp
                            << " entries=" << library.modules.size()
                            << " size=" << StringUtils::from_mem_size(library.total_size));

        return WG_OK;
    }

    Status ShaderLibrary::save_cache(const std::string& folder, const GfxShaderPlatform platform) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::save_cache");

        std::vector<ShaderModule> modules;
        dump_modules(platform, modules);

        FileShaderLibrary library;
        library.platform   = platform;
        library.timestamp  = DateTime::now();
        library.total_size = 0;
        library.modules.reserve(modules.size());

        for (ShaderModule& module : modules) {
            FileShaderModule& file_module = library.modules.emplace_back();
            file_module.bytecode          = std::move(module.bytecode);
            file_module.bytecode_hash     = std::move(module.bytecode_hash);
            file_module.source_hash       = std::move(module.source_hash);
            file_module.module_type       = std::move(module.module_type);
            file_module.name              = std::move(module.name);
            library.total_size += file_module.bytecode->size();
        }

        const std::string file_path = make_cache_file_name(folder, platform);
        ArchiveFile       archive;
        IoContext         context;

        if (!archive.open(file_path, {FileOpenMode::Out, FileOpenMode::Binary})) {
            WG_LOG_ERROR("failed to open shader library " << file_path << " for platform " << Enum::to_str(platform));
            return StatusCode::FailedOpenFile;
        }

        WG_CHECKED(archive.begin_compressed());
        WG_ARCHIVE_WRITE(context, archive, library);
        WG_CHECKED(archive.end_compressed());

        WG_LOG_INFO("save " << file_path
                            << " at=" << library.timestamp
                            << " entries=" << library.modules.size()
                            << " size=" << StringUtils::from_mem_size(library.total_size));

        return WG_OK;
    }

}// namespace wmoge

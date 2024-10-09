#include "shader_cache.hpp"

#include "core/date_time.hpp"
#include "core/log.hpp"
#include "grc/shader_library.hpp"
#include "io/stream_file.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "rtti/traits.hpp"

#include <cassert>
#include <sstream>
#include <vector>

namespace wmoge {

    struct FileShaderProgram {
        WG_RTTI_STRUCT(FileShaderProgram);

        buffered_vector<Sha256> modules;
        ShaderPermutation       permutation;
        Strid                   name;
    };

    WG_RTTI_STRUCT_BEGIN(FileShaderProgram) {
        WG_RTTI_FIELD(modules, {});
        WG_RTTI_FIELD(permutation, {});
        WG_RTTI_FIELD(name, {});
    }
    WG_RTTI_END;

    struct FileShaderProgramCache {
        WG_RTTI_STRUCT(FileShaderProgramCache);

        GfxShaderPlatform              platform;
        DateTime                       timestamp;
        std::vector<FileShaderProgram> programs;
    };

    WG_RTTI_STRUCT_BEGIN(FileShaderProgramCache) {
        WG_RTTI_FIELD(platform, {});
        WG_RTTI_FIELD(timestamp, {});
        WG_RTTI_FIELD(programs, {});
    }
    WG_RTTI_END;

    std::optional<ShaderProgram*> ShaderCacheMap::find_program(const ShaderPermutation& permutation) {
        auto query = m_programs.find(permutation);
        if (query != m_programs.end()) {
            return &(query->second);
        }

        return std::nullopt;
    }

    ShaderProgram& ShaderCacheMap::get_or_add_entry(const ShaderPermutation& permutation) {
        return m_programs[permutation];
    }

    void ShaderCacheMap::fit_program(const ShaderProgram& program) {
        ShaderProgram& new_entry = m_programs[program.permutation];
        new_entry                = program;
    }

    void ShaderCacheMap::dump_programs(std::vector<ShaderProgram>& out_programs) {
        for (const auto& entry : m_programs) {
            out_programs.push_back(entry.second);
        }
    }

    ShaderCache::ShaderCache() {
        rtti_type<FileShaderProgram>();
        rtti_type<FileShaderProgramCache>();
    }

    std::optional<ShaderProgram*> ShaderCache::find_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_maps[int(platform)].find_program(permutation);
    }

    ShaderProgram& ShaderCache::get_or_add_entry(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_maps[int(platform)].get_or_add_entry(permutation);
    }

    void ShaderCache::fit_program(GfxShaderPlatform platform, const ShaderProgram& program) {
        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        m_maps[int(platform)].fit_program(program);
    }

    void ShaderCache::dump_programs(GfxShaderPlatform platform, std::vector<ShaderProgram>& out_programs) {
        WG_AUTO_PROFILE_GRC("ShaderCache::dump_programs");

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        m_maps[int(platform)].dump_programs(out_programs);
    }

    std::string ShaderCache::make_cache_file_name(const std::string& folder, const std::string& name, GfxShaderPlatform platform) {
        const std::string cache_prefix = "shader_cache";
        const std::string cache_suffix = "scf";

        std::stringstream stream;
        stream << folder;
        stream << cache_prefix << ".";
        stream << name << ".";
        stream << GfxShaderPlatformFileName[int(platform)] << ".";
        stream << cache_suffix;
        return stream.str();
    }

    Status ShaderCache::load_cache(FileSystem* file_system, const std::string& file_path, GfxShaderPlatform platform, bool allow_missing) {
        WG_AUTO_PROFILE_GRC("ShaderCache::load_cache");

        IoStreamFile stream;
        IoContext    context;

        if (!stream.open(file_system, file_path, {FileOpenMode::In, FileOpenMode::Binary})) {
            if (allow_missing) {
                return WG_OK;
            }

            WG_LOG_ERROR("failed to open shader cache " << file_path << " for platform " << Enum::to_str(platform));
            return StatusCode::FailedOpenFile;
        }

        FileShaderProgramCache cache;

        WG_ARCHIVE_READ(context, stream, cache);

        if (cache.platform != platform) {
            WG_LOG_ERROR("mismatched platfrom in file " << file_path);
            return StatusCode::InvalidState;
        }

        for (FileShaderProgram& file_program : cache.programs) {
            ShaderProgram& program = m_maps[int(platform)].get_or_add_entry(file_program.permutation);
            program.modules        = std::move(file_program.modules);
            program.name           = std::move(file_program.name);
            program.permutation    = std::move(file_program.permutation);
            program.status         = ShaderStatus::InBytecode;
        }

        WG_LOG_INFO("load " << file_path << " created=" << cache.timestamp << " entries=" << cache.programs.size());

        return WG_OK;
    }

    Status ShaderCache::save_cache(FileSystem* file_system, const std::string& file_path, GfxShaderPlatform platform) {
        WG_AUTO_PROFILE_GRC("ShaderCache::save_cache");

        std::vector<ShaderProgram> programs;
        dump_programs(platform, programs);

        FileShaderProgramCache cache;
        cache.platform  = platform;
        cache.timestamp = DateTime::now();
        cache.programs.reserve(programs.size());

        for (ShaderProgram& program : programs) {
            if (program.status == ShaderStatus::InBytecode || program.status == ShaderStatus::Compiled) {
                FileShaderProgram& file_program = cache.programs.emplace_back();
                file_program.modules            = std::move(program.modules);
                file_program.name               = std::move(program.name);
                file_program.permutation        = std::move(program.permutation);
            }
        }

        IoStreamFile stream;
        IoContext    context;

        if (!stream.open(file_system, file_path, {FileOpenMode::Out, FileOpenMode::Binary})) {
            WG_LOG_ERROR("failed to open shader cache " << file_path << " for platform " << Enum::to_str(platform));
            return StatusCode::FailedOpenFile;
        }

        WG_ARCHIVE_WRITE(context, stream, cache);

        WG_LOG_INFO("save " << file_path << " at=" << cache.timestamp << " entries=" << cache.programs.size());

        return WG_OK;
    }

}// namespace wmoge
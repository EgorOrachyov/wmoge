#include "shader_cache.hpp"

#include "core/log.hpp"
#include "grc/shader_library.hpp"
#include "profiler/profiler.hpp"

#include <cassert>

namespace wmoge {

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

}// namespace wmoge
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

#include "glsl_include_processor.hpp"

#include "core/string_utils.hpp"

namespace wmoge {

    GlslIncludeProcessor::GlslIncludeProcessor(const ShaderCompilerEnv& env, FileSystem& file_system, bool collect_code) {
        m_file_system  = &file_system;
        m_env          = &env;
        m_collect_code = collect_code;
    }

    Status GlslIncludeProcessor::parse_file(const Strid& file) {
        std::string file_path = file.str();
        std::string content;
        bool        not_virtual = true;

        for (auto& rule : m_env->path_remappings) {
            if (StringUtils::is_starts_with(file_path, rule.first)) {
                file_path = StringUtils::find_replace_first(file_path, rule.first, rule.second);
                break;
            }
        }

        for (auto& virtual_include : m_env->virtual_includes) {
            if (StringUtils::is_starts_with(file_path, virtual_include.first)) {
                if (m_collect_code) {
                    content = virtual_include.second;
                }
                not_virtual = false;
                break;
            }
        }

        if (not_virtual && !m_file_system->read_file(file_path, content)) {
            WG_LOG_ERROR("failed read shader source " << file_path);
            return StatusCode::FailedRead;
        }

        std::stringstream stream(content);
        std::string       line;

        const auto include_prefix = std::string("#include ");
        const auto version_prefix = std::string("#version");

        while (std::getline(stream, line)) {
            StringUtils::find_replace_first(line, "\r", "");

            auto pragma = line.find(include_prefix);
            if (pragma != std::string::npos) {
                auto line_len  = line.length();
                auto cut_start = include_prefix.length() + 1;
                auto cut_count = line_len - cut_start - 2;

                const Strid include_file    = SID(line.substr(cut_start, cut_count));
                const auto  recursion_check = std::find(m_includes.begin(), m_includes.end(), include_file);

                if (recursion_check != m_includes.end()) {
                    continue;
                }

                m_includes.push_back(include_file);

                if (m_collect_code) {
                    m_result << "\n// Begin include file " << include_file << "\n";
                }

                if (!parse_file(include_file)) {
                    WG_LOG_ERROR("failed parse include file " << include_file);
                    return StatusCode::Error;
                }

                if (m_collect_code) {
                    m_result << "\n// End include file " << include_file << "\n";
                }

                continue;
            }

            pragma = line.find(version_prefix);
            if (pragma != std::string::npos && m_skip_version_pragma) {
                continue;
            }

            if (m_collect_code) {
                m_result << line << "\n";
            }
        }

        return StatusCode::Ok;
    }

}// namespace wmoge
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

namespace wmoge {

    GlslIncludeProcessor::GlslIncludeProcessor(std::string folder, FileSystem* file_system) {
        m_folder      = std::move(folder);
        m_file_system = file_system;
    }

    Status GlslIncludeProcessor::parse_file(const Strid& file) {
        const std::string file_path = m_folder + '/' + file.str();
        std::string       content;

        if (!m_file_system->read_file(file_path, content)) {
            WG_LOG_ERROR("failed read shader source " << file_path);
            return StatusCode::FailedRead;
        }

        std::stringstream stream(content);
        std::string       line;

        while (std::getline(stream, line)) {
            const auto prefix = std::string("#include ");
            auto       pragma = line.find(prefix);

            if (pragma != std::string::npos) {
                auto line_len  = line.length();
                auto cut_start = prefix.length() + 1;
                auto cut_count = line_len - cut_start - 1;

                const Strid include_file = SID(line.substr(cut_start, cut_count));

                const auto recursion_check = std::find(m_includes.begin(), m_includes.end(), include_file);

                if (recursion_check != m_includes.end()) {
                    continue;
                }

                if (!parse_file(include_file)) {
                    WG_LOG_ERROR("failed parse include file " << include_file);
                    return StatusCode::Error;
                }

                m_includes.push_back(include_file);

            } else {
                m_result << line;
            }
        }

        return StatusCode::Ok;
    }

}// namespace wmoge
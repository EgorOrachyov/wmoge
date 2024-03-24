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

#include "core/string_id.hpp"
#include "platform/file_system.hpp"

#include <sstream>
#include <string>

namespace wmoge {

    /**
     * @class GlslIncludeProcessor
     * @brief Process glsl-like syntax includes and emits final file with includes list
    */
    class GlslIncludeProcessor {
    public:
        GlslIncludeProcessor(std::string folder, FileSystem* file_system);

        /**
         * Recursively parse file collecting all includes
         * 
         * @param file Starting file to parse
         * 
         * @result Ok on success
        */
        Status parse_file(const Strid& file);

        std::vector<Strid>& get_includes() { return m_includes; }
        std::string         get_result() { return m_result.str(); }

    private:
        std::vector<Strid> m_includes;
        std::stringstream  m_result;
        std::string        m_folder;
        FileSystem*        m_file_system;
        bool               m_skip_version_pragma = true;
    };

}// namespace wmoge
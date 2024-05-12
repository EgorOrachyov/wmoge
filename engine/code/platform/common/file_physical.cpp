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

#include "file_physical.hpp"

namespace wmoge {

    Status FilePhysical::open(const std::filesystem::path& path, const FileOpenModeFlags& mode) {
        std::ios::openmode openmode{};

        if (mode.get(FileOpenMode::In)) {
            openmode = openmode | std::ios::in;
        }
        if (mode.get(FileOpenMode::Out)) {
            openmode = openmode | std::ios::out;
        }
        if (mode.get(FileOpenMode::Binary)) {
            openmode = openmode | std::ios::binary;
        }

        m_stream.open(path, openmode);

        if (!m_stream.is_open()) {
            return StatusCode::FailedOpenFile;
        }

        return WG_OK;
    }

    Status FilePhysical::nread(void* buffer, std::size_t bytes) {
        m_stream.read(reinterpret_cast<char*>(buffer), std::streamsize(bytes));
        return WG_OK;
    }

    Status FilePhysical::nwrite(const void* buffer, std::size_t bytes) {
        m_stream.write(reinterpret_cast<const char*>(buffer), std::streamsize(bytes));
        return WG_OK;
    }

    Status FilePhysical::eof(bool& is_eof) {
        is_eof = m_stream.eof();
        return WG_OK;
    }

    Status FilePhysical::size(std::size_t& out_size) {
        std::size_t pos = m_stream.tellg();
        m_stream.seekg(0, std::ios::end);
        out_size = m_stream.tellg();
        m_stream.seekg(std::streampos(pos), std::ios::beg);
        return WG_OK;
    }

}// namespace wmoge
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

#include "stream_file.hpp"

#include "platform/file_system.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

    IoStreamFile::IoStreamFile() {
        m_can_read  = false;
        m_can_write = false;
    }

    Status IoStreamFile::set(Ref<File> file, FileOpenModeFlags flags) {
        assert(file);

        m_file = std::move(file);

        m_can_read  = flags.get(FileOpenMode::In);
        m_can_write = flags.get(FileOpenMode::Out);

        assert(m_can_read && !m_can_write || m_can_write && !m_can_read);

        return WG_OK;
    }

    Status IoStreamFile::open(FileSystem* file_system, const std::string& file_path, FileOpenModeFlags flags) {
        Ref<File> file;
        WG_CHECKED(file_system->open_file(file_path, file, flags));
        return set(std::move(file), flags);
    }

    Status IoStreamFile::nwrite(std::size_t num_bytes, const void* bytes) {
        assert(m_file);
        assert(m_can_write);
        return m_file->nwrite(bytes, num_bytes);
    }

    Status IoStreamFile::nread(std::size_t num_bytes, void* bytes) {
        assert(m_file);
        assert(m_can_read);
        return m_file->nread(bytes, num_bytes);
    }

}// namespace wmoge
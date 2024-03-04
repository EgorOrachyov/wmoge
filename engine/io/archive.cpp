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

#include "archive.hpp"

namespace wmoge {

    Status archive_read(Archive& archive, bool& value) {
        char v;
        WG_ARCHIVE_READ(archive, v);
        value = v != 0;
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const bool& value) {
        char v = value ? 1 : 0;
        WG_ARCHIVE_WRITE(archive, v);
        return StatusCode::Ok;
    }

    Status archive_write(Archive& archive, const Strid& value) {
        WG_ARCHIVE_WRITE(archive, value.str());
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const std::string& value) {
        std::size_t len = value.length();
        WG_ARCHIVE_WRITE(archive, len);
        return archive.nwrite(int(value.length() * sizeof(char)), value.data());
    }

    Status archive_read(Archive& archive, Strid& value) {
        std::string str;
        WG_ARCHIVE_READ(archive, str);
        value = Strid(str);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, std::string& value) {
        std::size_t len;
        WG_ARCHIVE_READ(archive, len);
        value.resize(len);
        return archive.nread(int(value.length() * sizeof(char)), value.data());
    }

}// namespace wmoge
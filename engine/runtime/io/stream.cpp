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

#include "stream.hpp"

namespace wmoge {

    Status stream_read(IoContext& context, IoStream& stream, bool& value) {
        char v;
        WG_ARCHIVE_READ(context, stream, v);
        value = v != 0;
        return WG_OK;
    }
    Status stream_write(IoContext& context, IoStream& stream, const bool& value) {
        char v = value ? 1 : 0;
        WG_ARCHIVE_WRITE(context, stream, v);
        return WG_OK;
    }

    Status stream_read(IoContext& context, IoStream& stream, Strid& value) {
        std::string str;
        WG_ARCHIVE_READ(context, stream, str);
        value = Strid(str);
        return WG_OK;
    }
    Status stream_write(IoContext& context, IoStream& stream, const Strid& value) {
        WG_ARCHIVE_WRITE(context, stream, value.str());
        return WG_OK;
    }

    Status stream_read(IoContext& context, IoStream& stream, std::string& value) {
        std::size_t len;
        WG_ARCHIVE_READ(context, stream, len);
        value.resize(len);
        return stream.nread(int(value.length() * sizeof(char)), value.data());
    }
    Status stream_write(IoContext& context, IoStream& stream, const std::string& value) {
        std::size_t len = value.length();
        WG_ARCHIVE_WRITE(context, stream, len);
        return stream.nwrite(int(value.length() * sizeof(char)), value.data());
    }

    Status stream_read(IoContext& context, IoStream& stream, Status& value) {
        return stream_read(context, stream, value.code());
    }
    Status stream_write(IoContext& context, IoStream& stream, const Status& value) {
        return stream_write(context, stream, value.code());
    }

}// namespace wmoge
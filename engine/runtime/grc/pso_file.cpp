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

#include "pso_file.hpp"

#include "io/stream_file.hpp"

namespace wmoge {

    Status PsoFile::read(const std::string& path, FilePsoData& pso_data) {
        IoStreamFile stream;
        WG_CHECKED(stream.open(path, {FileOpenMode::In, FileOpenMode::Binary}));

        IoContext context;
        WG_ARCHIVE_READ(context, stream, pso_data);

        return WG_OK;
    }

    Status PsoFile::write(const std::string& path, const FilePsoData& pso_data) {
        IoStreamFile stream;
        WG_CHECKED(stream.open(path, {FileOpenMode::Out, FileOpenMode::Binary}));

        IoContext context;
        WG_ARCHIVE_WRITE(context, stream, pso_data);

        return WG_OK;
    }

}// namespace wmoge
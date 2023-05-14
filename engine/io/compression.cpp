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

#include "compression.hpp"

#include "debug/profiler.hpp"

#include <lz4.hpp>

namespace wmoge {

    bool Compression::compress_lz4(const void* in, int size, std::vector<std::uint8_t>& out) {
        WG_AUTO_PROFILE_IO("Compression::compress_lz4");

        const int max_compressed_size = LZ4_compressBound(size);
        out.resize(max_compressed_size);

        const int compressed_size = LZ4_compress_default(reinterpret_cast<const char*>(in), reinterpret_cast<char*>(out.data()), size, max_compressed_size);
        if (compressed_size > 0) {
            out.resize(compressed_size);
            return true;
        }

        return false;
    }

    bool Compression::decompress_lz4(const void* in, int compressed_size, int decompressed_size, std::uint8_t* out) {
        WG_AUTO_PROFILE_IO("Compression::decompress_lz4");

        return LZ4_decompress_safe(reinterpret_cast<const char*>(in), reinterpret_cast<char*>(out), compressed_size, decompressed_size) > 0;
    }

}// namespace wmoge
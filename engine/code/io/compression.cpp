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

#include "profiler/profiler_cpu.hpp"

#include <lz4.hpp>

namespace wmoge {

    Status Compression::estimate_lz4(array_view<const std::uint8_t> in, std::size_t& max_compressed_size) {
        WG_PROFILE_CPU_IO("Compression::estimate_lz4");

        max_compressed_size = LZ4_compressBound(static_cast<int>(in.size()));

        return WG_OK;
    }

    Status Compression::compress_lz4(array_view<const std::uint8_t> in, array_view<std::uint8_t> out, std::size_t& compressed_size) {
        WG_PROFILE_CPU_IO("Compression::compress_lz4");

        compressed_size = LZ4_compress_default(reinterpret_cast<const char*>(in.data()), reinterpret_cast<char*>(out.data()), static_cast<int>(in.size()), static_cast<int>(out.size()));
        if (compressed_size > 0) {
            return WG_OK;
        }

        return StatusCode::FailedCompress;
    }

    Status Compression::decompress_lz4(array_view<const std::uint8_t> in, array_view<std::uint8_t> out) {
        WG_PROFILE_CPU_IO("Compression::decompress_lz4");

        const int decompressed_bytes = LZ4_decompress_safe(reinterpret_cast<const char*>(in.data()), reinterpret_cast<char*>(out.data()), static_cast<int>(in.size()), static_cast<int>(out.size()));
        if (decompressed_bytes == out.size()) {
            return StatusCode::Ok;
        }

        return StatusCode::FailedDecompress;
    }

}// namespace wmoge
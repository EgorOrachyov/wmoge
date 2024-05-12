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

#ifndef WMOGE_COMPRESSION_HPP
#define WMOGE_COMPRESSION_HPP

#include "core/status.hpp"

#include <cinttypes>
#include <vector>

namespace wmoge {

    /**
     * @class Compression
     * @brief Provides convenient wrapper for fast lz4 library
     */
    class Compression {
    public:
        /**
         * @brief Compress provided buffer
         *
         * @param in buffer to compress
         * @param size buffer size
         * @param out output buffer
         *
         * @return Ok on success
         */
        static Status compress_lz4(const void* in, int size, std::vector<std::uint8_t>& out);

        /**
         * @brief Decompress provided buffer
         *
         * @param in buffer to decompress
         * @param compressed_size size of compressed buffer
         * @param decompressed_size decompressed data size
         * @param out buffer for decompressed data
         *
         * @return Ok in success
         */
        static Status decompress_lz4(const void* in, int compressed_size, int decompressed_size, std::uint8_t* out);
    };

}// namespace wmoge

#endif//WMOGE_COMPRESSION_HPP

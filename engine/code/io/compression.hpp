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

#include "core/array_view.hpp"
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
         * @brief Estimates upper boundary for compressed buffer size
         * 
         * @param in buffer to compress
         * @param max_compressed_size estimate
         * 
         * @return Ok on success
        */
        static Status estimate_lz4(array_view<const std::uint8_t> in, std::size_t& max_compressed_size);

        /**
         * @brief Compress provided buffer
         *
         * @param in buffer to compress
         * @param out output buffer
         * @param compressed_size copressed data size
         *
         * @return Ok on success
         */
        static Status compress_lz4(array_view<const std::uint8_t> in, array_view<std::uint8_t> out, std::size_t& compressed_size);

        /**
         * @brief Decompress provided buffer
         *
         * @param in buffer to decompress
         * @param out buffer for decompressed data
         *
         * @return Ok in success
         */
        static Status decompress_lz4(array_view<const std::uint8_t> in, array_view<std::uint8_t> out);
    };

}// namespace wmoge

#endif//WMOGE_COMPRESSION_HPP

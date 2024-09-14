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

#include "io/property_tree.hpp"
#include "io/stream.hpp"

#include <array>
#include <cinttypes>
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @class Sha256
     * @brief SHA 256 bit hash
    */
    struct Sha256 {
        static constexpr unsigned int NUM_BITS   = 256;
        static constexpr unsigned int NUM_BYTES  = NUM_BITS / 8;
        static constexpr unsigned int NUM_WORDS  = NUM_BYTES / 4;
        static constexpr unsigned int NUM_HEXDIG = NUM_BITS / 4;

        Sha256() = default;
        Sha256(const std::string& s);

        bool operator==(const Sha256& other) const;
        bool operator!=(const Sha256& other) const;

        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::size_t hash() const;

        std::uint32_t values[NUM_WORDS] = {0};
    };

    static_assert(std::is_trivially_destructible_v<Sha256>, "Sha256 must be trivial as ptr or int");

    Status tree_read(IoContext& context, IoPropertyTree& tree, Sha256& sha);
    Status tree_write(IoContext& context, IoPropertyTree& tree, const Sha256& sha);
    Status stream_read(IoContext& context, IoStream& stream, Sha256& sha);
    Status stream_write(IoContext& context, IoStream& stream, const Sha256& sha);

    inline std::ostream& operator<<(std::ostream& stream, const Sha256& sha) {
        stream << sha.to_string();
        return stream;
    }

    /**
     * @class Sha256Builder
     * @brief Builder for sha256 hash
     * 
     * @see https://github.com/System-Glitch/SHA256/
    */
    class Sha256Builder {
    public:
        Sha256Builder();

        Sha256Builder& hash(const void* buffer, std::size_t size);
        Sha256         get();

    private:
        void update(const uint8_t* data, size_t length);
        void update(const std::string& data);

        std::array<uint8_t, 32> digest();

        static uint32_t rotr(uint32_t x, uint32_t n);
        static uint32_t choose(uint32_t e, uint32_t f, uint32_t g);
        static uint32_t majority(uint32_t a, uint32_t b, uint32_t c);
        static uint32_t sig0(uint32_t x);
        static uint32_t sig1(uint32_t x);

        void transform();
        void pad();
        void revert(std::array<uint8_t, 32>& hash);

    private:
        uint8_t  m_data[64];
        uint32_t m_blocklen;
        uint64_t m_bitlen;
        uint32_t m_state[8];//A, B, C, D, E, F, G, H
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::Sha256> {
    public:
        std::size_t operator()(const wmoge::Sha256& sha) const {
            return sha.hash();
        }
    };

}// namespace std
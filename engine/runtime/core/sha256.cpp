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

#include "sha256.hpp"

#include <cstddef>
#include <cstring>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>

namespace wmoge {

    static const char G_DEC_TO_HEX[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    Sha256::Sha256(const std::string& str) {
        std::stringstream s(str);

        for (std::uint32_t i = 0; i < NUM_HEXDIG; i++) {
            const std::uint32_t word  = i / 8;
            const std::uint32_t shift = (i % 8) * 4;

            char digit;
            s >> digit;

            values[word] |= static_cast<std::uint32_t>((digit - '0')) << shift;
        }
    }

    bool Sha256::operator==(const Sha256& other) const {
        for (int i = 0; i < NUM_WORDS; i++) {
            if (values[i] != other.values[i]) {
                return false;
            }
        }
        return true;
    }

    bool Sha256::operator!=(const Sha256& other) const {
        for (int i = 0; i < NUM_WORDS; i++) {
            if (values[i] != other.values[i]) {
                return true;
            }
        }
        return false;
    }

    std::string Sha256::to_string() const {
        std::stringstream s;

        for (std::uint32_t i = 0; i < NUM_HEXDIG; i++) {
            const std::uint32_t word  = i / 8;
            const std::uint32_t shift = (i % 8) * 4;

            const int digit = (values[word] >> shift) & 0xf;
            s << G_DEC_TO_HEX[digit];
        }

        return s.str();
    }

    std::size_t Sha256::hash() const {
        auto        hasher     = std::hash<std::size_t>();
        std::size_t hash_value = hasher(values[0]);

        for (int i = 1; i < NUM_WORDS; i++) {
            hash_value = hasher(values[i]);
        }

        return hash_value;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, Sha256& sha) {
        std::string s;
        WG_CHECKED(yaml_read(context, node, s));
        sha = Sha256(s);
        return WG_OK;
    }

    Status yaml_write(IoContext& context, YamlNodeRef node, const Sha256& sha) {
        return yaml_write(context, node, sha.to_string());
    }
    Status stream_read(IoContext& context, IoStream& stream, Sha256& sha) {
        return stream.nread(sizeof(sha), &sha);
    }
    Status stream_write(IoContext& context, IoStream& stream, const Sha256& sha) {
        return stream.nwrite(sizeof(sha), &sha);
    }

    static constexpr std::array<uint32_t, 64> K = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
            0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
            0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
            0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
            0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
            0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
            0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
            0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    Sha256Builder::Sha256Builder() : m_blocklen(0), m_bitlen(0) {
        m_state[0] = 0x6a09e667;
        m_state[1] = 0xbb67ae85;
        m_state[2] = 0x3c6ef372;
        m_state[3] = 0xa54ff53a;
        m_state[4] = 0x510e527f;
        m_state[5] = 0x9b05688c;
        m_state[6] = 0x1f83d9ab;
        m_state[7] = 0x5be0cd19;
    }

    Sha256Builder& Sha256Builder::hash(const void* buffer, std::size_t size) {
        update((std::uint8_t*) buffer, size);
        return *this;
    }

    Sha256 Sha256Builder::get() {
        const std::array<std::uint8_t, 32> d = digest();
        static_assert(sizeof(Sha256) == sizeof(std::array<std::uint8_t, 32>), "Hash size must match");

        Sha256 result;

        for (int i = 0; i < Sha256::NUM_WORDS; i++) {
            result.values[i] = ((const std::uint32_t*) d.data())[i];
        }

        return result;
    }

    void Sha256Builder::update(const uint8_t* data, size_t length) {
        for (size_t i = 0; i < length; i++) {
            m_data[m_blocklen++] = data[i];
            if (m_blocklen == 64) {
                transform();

                // End of the block
                m_bitlen += 512;
                m_blocklen = 0;
            }
        }
    }

    void Sha256Builder::update(const std::string& data) {
        update(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
    }

    std::array<uint8_t, 32> Sha256Builder::digest() {
        std::array<uint8_t, 32> hash;

        pad();
        revert(hash);

        return hash;
    }

    uint32_t Sha256Builder::rotr(uint32_t x, uint32_t n) {
        return (x >> n) | (x << (32 - n));
    }

    uint32_t Sha256Builder::choose(uint32_t e, uint32_t f, uint32_t g) {
        return (e & f) ^ (~e & g);
    }

    uint32_t Sha256Builder::majority(uint32_t a, uint32_t b, uint32_t c) {
        return (a & (b | c)) | (b & c);
    }

    uint32_t Sha256Builder::sig0(uint32_t x) {
        return Sha256Builder::rotr(x, 7) ^ Sha256Builder::rotr(x, 18) ^ (x >> 3);
    }

    uint32_t Sha256Builder::sig1(uint32_t x) {
        return Sha256Builder::rotr(x, 17) ^ Sha256Builder::rotr(x, 19) ^ (x >> 10);
    }

    void Sha256Builder::transform() {
        uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
        uint32_t state[8];

        for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) {// Split data in 32 bit blocks for the 16 first words
            m[i] = (m_data[j] << 24) | (m_data[j + 1] << 16) | (m_data[j + 2] << 8) | (m_data[j + 3]);
        }

        for (uint8_t k = 16; k < 64; k++) {// Remaining 48 blocks
            m[k] = Sha256Builder::sig1(m[k - 2]) + m[k - 7] + Sha256Builder::sig0(m[k - 15]) + m[k - 16];
        }

        for (uint8_t i = 0; i < 8; i++) {
            state[i] = m_state[i];
        }

        for (uint8_t i = 0; i < 64; i++) {
            maj  = Sha256Builder::majority(state[0], state[1], state[2]);
            xorA = Sha256Builder::rotr(state[0], 2) ^ Sha256Builder::rotr(state[0], 13) ^ Sha256Builder::rotr(state[0], 22);

            ch = choose(state[4], state[5], state[6]);

            xorE = Sha256Builder::rotr(state[4], 6) ^ Sha256Builder::rotr(state[4], 11) ^ Sha256Builder::rotr(state[4], 25);

            sum  = m[i] + K[i] + state[7] + ch + xorE;
            newA = xorA + maj + sum;
            newE = state[3] + sum;

            state[7] = state[6];
            state[6] = state[5];
            state[5] = state[4];
            state[4] = newE;
            state[3] = state[2];
            state[2] = state[1];
            state[1] = state[0];
            state[0] = newA;
        }

        for (uint8_t i = 0; i < 8; i++) {
            m_state[i] += state[i];
        }
    }

    void Sha256Builder::pad() {

        uint64_t i   = m_blocklen;
        uint8_t  end = m_blocklen < 56 ? 56 : 64;

        m_data[i++] = 0x80;// Append a bit 1
        while (i < end) {
            m_data[i++] = 0x00;// Pad with zeros
        }

        if (m_blocklen >= 56) {
            transform();
            std::memset(m_data, 0, 56);
        }

        // Append to the padding the total message's length in bits and transform.
        m_bitlen += m_blocklen * 8;

        m_data[63] = static_cast<uint8_t>((m_bitlen >> 0) & 0xff);
        m_data[62] = static_cast<uint8_t>((m_bitlen >> 8) & 0xff);
        m_data[61] = static_cast<uint8_t>((m_bitlen >> 16) & 0xff);
        m_data[60] = static_cast<uint8_t>((m_bitlen >> 24) & 0xff);
        m_data[59] = static_cast<uint8_t>((m_bitlen >> 32) & 0xff);
        m_data[58] = static_cast<uint8_t>((m_bitlen >> 40) & 0xff);
        m_data[57] = static_cast<uint8_t>((m_bitlen >> 48) & 0xff);
        m_data[56] = static_cast<uint8_t>((m_bitlen >> 56) & 0xff);

        transform();
    }

    void Sha256Builder::revert(std::array<uint8_t, 32>& hash) {
        // SHA uses big endian byte ordering
        // Revert all bytes
        for (uint8_t i = 0; i < 4; i++) {
            for (uint8_t j = 0; j < 8; j++) {
                hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
            }
        }
    }

}// namespace wmoge
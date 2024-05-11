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

#include "core/buffered_vector.hpp"
#include "io/archive.hpp"
#include "io/yaml.hpp"

#include <bitset>
#include <initializer_list>
#include <sstream>
#include <vector>

namespace wmoge {

    /**
     * @class Mask
     * @brief Typed mask to test safely enum flags
     *
     * @tparam T type of flags
     * @tparam size flags count (default is 32)
     */
    template<typename T, int size = 32>
    struct Mask {
        using BitsetType = std::bitset<size>;

        Mask() = default;

        Mask(const std::bitset<size>& input) {
            bits = input;
        }

        Mask(const std::initializer_list<T>& elements) {
            for (auto e : elements) set(e);
        }

        bool operator==(const Mask& other) const { return bits == other.bits; }
        bool operator!=(const Mask& other) const { return bits != other.bits; }

        Mask operator&(const Mask& other) const { return Mask(bits & other.bits); }
        Mask operator|(const Mask& other) const { return Mask(bits | other.bits); }

        Mask& operator&=(const Mask& other) {
            bits &= other.bits;
            return *this;
        }
        Mask& operator|=(const Mask& other) {
            bits |= other.bits;
            return *this;
        }

        bool get(T flag) const { return bits.test(static_cast<int>(flag)); }
        void set(T flag, bool value = true) { bits.set(static_cast<int>(flag), value); }

        template<typename Function>
        void for_each(Function&& function) const {
            for (int i = 0; i < size; i++) {
                if (bits.test(i)) {
                    T value = static_cast<T>(i);
                    function(i, value);
                }
            }
        }

        [[nodiscard]] std::string to_string() const {
            std::stringstream stream;
            stream << "[";
            for_each([&](int, T value) {
                stream << Enum::to_str(value) << ",";
            });
            stream << "]";
            return stream.str();
        }

        std::bitset<size> bits;
    };

    template<typename Stream, typename T, int size>
    Stream& operator<<(Stream& stream, const Mask<T, size>& mask) {
        stream << mask.bits;
        return stream;
    }

    template<typename T, int size>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, Mask<T, size>& mask) {
        buffered_vector<T, size> flags;
        WG_YAML_READ(context, node, flags);

        for (auto flag : flags) {
            mask.set(flag);
        }

        return StatusCode::Ok;
    }

    template<typename T, int size>
    Status yaml_write(IoContext& context, YamlNodeRef node, const Mask<T, size>& mask) {
        buffered_vector<T, size> flags;

        mask.for_each([&](int, T flag) {
            flags.push_back(flag);
        });

        WG_YAML_WRITE(context, node, flags);
        return StatusCode::Ok;
    }

    template<typename T, int size>
    Status archive_read(IoContext& context, Archive& archive, Mask<T, size>& mask) {
        return archive.nread(sizeof(Mask<T, size>), &mask);
    }

    template<typename T, int size>
    Status archive_write(IoContext& context, Archive& archive, const Mask<T, size>& mask) {
        return archive.nwrite(sizeof(Mask<T, size>), &mask);
    }

}// namespace wmoge
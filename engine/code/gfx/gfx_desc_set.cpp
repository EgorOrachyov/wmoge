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

#include "gfx_desc_set.hpp"

#include <bitset>
#include <functional>

namespace wmoge {

    bool GfxDescBinging::operator==(const GfxDescBinging& other) const {
        return type == other.type &&
               binding == other.binding &&
               count == other.count;
    }
    bool GfxDescBinging::operator!=(const GfxDescBinging& other) const {
        return type != other.type ||
               binding != other.binding ||
               count != other.count;
    }
    std::size_t GfxDescBinging::hash() const {
        return std::hash<int>()(static_cast<int>(type)) ^
               std::hash<std::int16_t>()(binding) ^
               std::hash<std::int16_t>()(count);
    }

    bool GfxDescBindPoint::operator==(const GfxDescBindPoint& other) const {
        return type == other.type &&
               binding == other.binding &&
               array_element == other.array_element;
    }
    bool GfxDescBindPoint::operator!=(const GfxDescBindPoint& other) const {
        return type != other.type ||
               binding != other.binding ||
               array_element != other.array_element;
    }
    std::size_t GfxDescBindPoint::hash() const {
        return std::hash<int>()(static_cast<int>(type)) ^
               std::hash<std::int16_t>()(binding) ^
               std::hash<std::int16_t>()(array_element);
    }

    void GfxDescSet::fill_required_layout(const GfxDescSetResources& resources, GfxDescSetLayoutDesc& layout) {
        std::bitset<GfxLimits::MAX_DESC_SET_SIZE>                used_bindings;
        std::array<GfxBindingType, GfxLimits::MAX_DESC_SET_SIZE> bindings_types{};
        std::array<std::int16_t, GfxLimits::MAX_DESC_SET_SIZE>   bindings_count{};

        for (const auto& [point, value] : resources) {
            assert(point.binding < GfxLimits::MAX_DESC_SET_SIZE);

            used_bindings.set(point.binding);
            bindings_types[point.binding] = point.type;
            bindings_count[point.binding] += 1;
        }

        layout.clear();

        for (int i = 0; i < GfxLimits::MAX_DESC_SET_SIZE; i++) {
            if (used_bindings[i]) {
                GfxDescBinging binging;
                binging.type    = bindings_types[i];
                binging.binding = std::int16_t(i);
                binging.count   = bindings_count[i];
                layout.push_back(binging);
            }
        }
    }

}// namespace wmoge
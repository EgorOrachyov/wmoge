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

#ifndef WMOGE_PFX_STORAGE_HPP
#define WMOGE_PFX_STORAGE_HPP

#include "core/mask.hpp"
#include "math/color.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"

#include <array>
#include <cinttypes>
#include <vector>

namespace wmoge {

    /**
     * @class PfxAttribute
     * @brief Attributes of the particle system
     */
    enum class PfxAttribute {
        /** 2-float global position in 2d space of particle */
        Pos2d,
        /** 2-float global particle velocity in 2d space */
        Vel2d,
        /** 4-float rgba color of particle */
        Color,
        /** 1-float global 2d space counterclockwise rotation around center */
        Angle,
        /** 1-float size scale of particle */
        Size,
        /** 1-float particle age in seconds */
        Time,
        /** 1-float normalized (0 to 1) age accordingly to time to live */
        TimeNorm,
        /** 1-float particle time to live total in seconds */
        TimeToLive,
        /** Total features count */
        Total
    };

    /**
     * @class PfxAttributes
     * @brief Attributes mask of the particle system
     */
    using PfxAttributes = Mask<PfxAttribute>;

    /**
     * @class PfxView
     * @brief View to update particles attributes
     *
     * @tparam T Type of attribute
     */
    template<typename T>
    class PfxView {
    public:
        PfxView(int stride, std::uint8_t* data) {
            m_stride = stride;
            m_data   = data;
        }

        T& operator[](int i) const {
            return *(T*) (m_data + m_stride * i);
        }

    private:
        int           m_stride = 0;
        std::uint8_t* m_data   = nullptr;
    };

    /**
     * @class PfxRange
     * @brief Circular range wrapper for simplified iterating
     */
    class PfxRange {
    public:
        PfxRange(int from, int count, int size) {
            this->from  = from;
            this->count = count;
            this->size  = size;
        }

        struct Iterator {
            bool operator!=(const Iterator& other) const {
                return count != other.count;
            }
            int operator*() {
                return current;
            }
            void operator++() {
                if (count > 0) {
                    count -= 1;
                    current = (current + 1) % size;
                }
            }

            int current = -1;
            int count   = -1;
            int size    = -1;
        };

        Iterator begin() { return {from, count, size}; }
        Iterator end() { return {0, 0, 0}; }

        int from  = -1;
        int count = -1;
        int size  = -1;
    };

    /**
     * @class PfxStorage
     * @brief Storage for particles of a particular pfx emitter component
     */
    class PfxStorage {
    public:
        static const int TOTAL_ATTRIBUTES = static_cast<int>(PfxAttribute::Total);

        PfxStorage(PfxAttributes attributes, int capacity);

        PfxView<Vec2f>             get_pos2d();
        PfxView<Vec2f>             get_vel2d();
        PfxView<Color4f>           get_color();
        PfxView<float>             get_angle();
        PfxView<float>             get_size();
        PfxView<float>             get_time();
        PfxView<float>             get_time_norm();
        PfxView<float>             get_time_to_live();
        std::vector<std::uint8_t>& get_data();
        int                        get_capacity() const;
        int                        get_stride() const;
        PfxAttributes              get_attributes() const;

    private:
        std::array<int, TOTAL_ATTRIBUTES> m_sizes;
        std::array<int, TOTAL_ATTRIBUTES> m_offsets;
        std::vector<std::uint8_t>         m_data;
        int                               m_capacity = 0;
        int                               m_stride   = 0;
        PfxAttributes                     m_attributes;
    };

}// namespace wmoge

#endif//WMOGE_PFX_STORAGE_HPP

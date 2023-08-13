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

#ifndef WMOGE_AABB_HPP
#define WMOGE_AABB_HPP

#include "io/yaml.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class TAabb
     * @brief Axis-aligned bounding box
     *
     * @tparam T Type of values
     */
    template<typename T>
    class TAabb {
    public:
        TAabb() = default;

        TAabb(const TVecN<T, 3>& in_pos, const TVecN<T, 3>& in_size_half) {
            pos       = in_pos;
            size_half = in_size_half;
        }

        [[nodiscard]] TAabb join(const TAabb& other) {
            auto join_min = Vec3f::min(min(), other.min());
            auto join_max = Vec3f::max(max(), other.max());
            return TAabb<T>((join_max + join_min) * static_cast<T>(0.5), (join_max - join_min) * static_cast<T>(0.5));
        }

        [[nodiscard]] float distance(const TVecN<T, 3>& point) const {
            return (center() - point).length();
        }

        TVecN<T, 3> center() const { return pos; }
        TVecN<T, 3> extent() const { return size_half; }
        TVecN<T, 3> min() const { return pos - size_half; }
        TVecN<T, 3> max() const { return pos + size_half; }

    public:
        TVecN<T, 3> pos;
        TVecN<T, 3> size_half;
    };

    using Aabbf = TAabb<float>;

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, TAabb<T>& aabb) {
        WG_YAML_READ_AS(node, "pos", aabb.pos);
        WG_YAML_READ_AS(node, "size_half", aabb.size_half);
        return StatusCode::Ok;
    }
    template<typename T>
    Status yaml_write(YamlNodeRef node, const TAabb<T>& aabb) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "pos", aabb.pos);
        WG_YAML_WRITE_AS(node, "size_half", aabb.size_half);
        return StatusCode::Ok;
    }

}// namespace wmoge

#endif//WMOGE_AABB_HPP

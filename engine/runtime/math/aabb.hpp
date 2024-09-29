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

#include "io/stream.hpp"
#include "io/tree.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"

#include <array>

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

        [[nodiscard]] TAabb fit(const TVecN<T, 3>& point) {
            auto join_min = Vec3f::min(min(), point);
            auto join_max = Vec3f::max(max(), point);
            return TAabb<T>((join_max + join_min) * static_cast<T>(0.5), (join_max - join_min) * static_cast<T>(0.5));
        }

        [[nodiscard]] float distance(const TVecN<T, 3>& point) const {
            return (center() - point).length();
        }

        [[nodiscard]] std::array<TVecN<T, 3>, 8> vertices() const {
            std::array<TVecN<T, 3>, 8> res;
            res[0] = TVecN<T, 3>(pos.x() + size_half.x(), pos.y() + size_half.y(), pos.z() + size_half.z());
            res[1] = TVecN<T, 3>(pos.x() + size_half.x(), pos.y() + size_half.y(), pos.z() - size_half.z());
            res[2] = TVecN<T, 3>(pos.x() + size_half.x(), pos.y() - size_half.y(), pos.z() + size_half.z());
            res[3] = TVecN<T, 3>(pos.x() + size_half.x(), pos.y() - size_half.y(), pos.z() - size_half.z());
            res[4] = TVecN<T, 3>(pos.x() - size_half.x(), pos.y() + size_half.y(), pos.z() + size_half.z());
            res[5] = TVecN<T, 3>(pos.x() - size_half.x(), pos.y() + size_half.y(), pos.z() - size_half.z());
            res[6] = TVecN<T, 3>(pos.x() - size_half.x(), pos.y() - size_half.y(), pos.z() + size_half.z());
            res[7] = TVecN<T, 3>(pos.x() - size_half.x(), pos.y() - size_half.y(), pos.z() - size_half.z());
            return res;
        }

        [[nodiscard]] TAabb transform(const TMatMxN<T, 4, 4>& m) const {
            std::array<TVecN<T, 3>, 8> v = vertices();

            for (int i = 0; i < 8; i++) {
                v[i] = TVecN<T, 3>(m * TVecN<T, 4>(v[i].x(), v[i].y(), v[i].z(), T(1)));
            }

            TAabb res;
            res.pos = v[0];

            for (int i = 1; i < 8; i++) {
                res = res.fit(v[i]);
            }

            return res;
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
    inline std::ostream& operator<<(std::ostream& ostream, const TAabb<T>& vec) {
        ostream << "(";
        ostream << "min=" << vec.min() << ", ";
        ostream << "max=" << vec.max();
        ostream << ")";
        return ostream;
    }

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, TAabb<T>& aabb) {
        WG_TREE_READ_AS(context, tree, "pos", aabb.pos);
        WG_TREE_READ_AS(context, tree, "size_half", aabb.size_half);
        return WG_OK;
    }
    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const TAabb<T>& aabb) {
        WG_TREE_MAP(tree);
        WG_TREE_WRITE_AS(context, tree, "pos", aabb.pos);
        WG_TREE_WRITE_AS(context, tree, "size_half", aabb.size_half);
        return WG_OK;
    }
    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, TAabb<T>& aabb) {
        WG_ARCHIVE_READ(context, stream, aabb.pos);
        WG_ARCHIVE_READ(context, stream, aabb.size_half);
        return WG_OK;
    }
    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const TAabb<T>& aabb) {
        WG_ARCHIVE_WRITE(context, stream, aabb.pos);
        WG_ARCHIVE_WRITE(context, stream, aabb.size_half);
        return WG_OK;
    }

}// namespace wmoge
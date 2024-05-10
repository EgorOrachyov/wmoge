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

#include "io/archive.hpp"
#include "io/yaml.hpp"
#include "math_utils.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <ostream>
#include <sstream>

namespace wmoge {

    template<typename T, int N>
    class TVecN {
    public:
        static_assert(N > 0, "Vector size must be more than 0");

        TVecN() noexcept = default;

        explicit TVecN(T x) : TVecN() {
            static_assert(N >= 1, "Out of bounds index assignment");
            values[0] = x;
        }

        TVecN(T x, T y) : TVecN() {
            static_assert(N >= 2, "Out of bounds index assignment");
            values[0] = x;
            values[1] = y;
        }

        TVecN(T x, T y, T z) : TVecN() {
            static_assert(N >= 3, "Out of bounds index assignment");
            values[0] = x;
            values[1] = y;
            values[2] = z;
        }

        TVecN(T x, T y, T z, T w) : TVecN() {
            static_assert(N >= 4, "Out of bounds index assignment");
            values[0] = x;
            values[1] = y;
            values[2] = z;
            values[3] = w;
        }

        template<int M>
        TVecN(const TVecN<T, M>& v, T a) : TVecN() {
            static_assert(N >= M + 1, "Out of bounds index assignment");
            for (int i = 0; i < M; i++) {
                values[i] = v.values[i];
            }
            values[M] = a;
        }

        template<int M>
        TVecN(T a, const TVecN<T, M>& v) : TVecN() {
            static_assert(N >= M + 1, "Out of bounds index assignment");
            values[0] = a;
            for (int i = 0; i < M; i++) {
                values[i + 1] = v.values[i];
            }
        }

        template<int M>
        TVecN(T a, T b, const TVecN<T, M>& v) : TVecN() {
            static_assert(N >= M + 2, "Out of bounds index assignment");
            values[0] = a;
            values[1] = b;
            for (int i = 0; i < M; i++) {
                values[i + 2] = v.values[i];
            }
        }

        TVecN(const std::initializer_list<T>& list) noexcept : TVecN<T, N>() {
            int i = 0;
            for (const auto& a : list) {
                if (i >= N) return;
                values[i] = a;
                i += 1;
            }
        }

        TVecN(const TVecN& other) noexcept {
            for (int i = 0; i < N; i++) {
                values[i] = other.values[i];
            }
        }

        TVecN(TVecN&& other) noexcept {
            for (int i = 0; i < N; i++) {
                values[i] = other.values[i];
            }
        }

        template<int M>
        explicit TVecN(const TVecN<T, M>& other) noexcept : TVecN<T, N>() {
            int i = 0;
            while (i < N && i < M) {
                values[i] = other.values[i];
                i += 1;
            }
        }

        TVecN& operator=(const TVecN& other) {
            if (this != &other) {
                for (int i = 0; i < N; i++) {
                    values[i] = other.values[i];
                }
            }
            return *this;
        }

        TVecN& operator=(TVecN&& other) noexcept {
            if (this != &other) {
                for (int i = 0; i < N; i++) {
                    values[i] = other.values[i];
                }
            }
            return *this;
        }

        TVecN operator+(const TVecN& other) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] + other.values[i];
            }

            return r;
        }

        TVecN operator-(const TVecN& other) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] - other.values[i];
            }

            return r;
        }

        TVecN operator*(const TVecN& other) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] * other.values[i];
            }

            return r;
        }

        TVecN operator/(const TVecN& other) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] / other.values[i];
            }

            return r;
        }

        TVecN operator+(T a) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] + a;
            }

            return r;
        }

        TVecN operator-(T a) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] - a;
            }

            return r;
        }

        TVecN operator*(T a) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] * a;
            }

            return r;
        }

        TVecN operator/(T a) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = values[i] / a;
            }

            return r;
        }

        TVecN& operator+=(const TVecN& other) {
            for (int i = 0; i < N; i++) {
                values[i] += other.values[i];
            }

            return *this;
        }

        TVecN& operator-=(const TVecN& other) {
            for (int i = 0; i < N; i++) {
                values[i] -= other.values[i];
            }

            return *this;
        }

        TVecN& operator*=(const TVecN& other) {
            for (int i = 0; i < N; i++) {
                values[i] *= other.values[i];
            }

            return *this;
        }

        TVecN& operator/=(const TVecN& other) {
            for (int i = 0; i < N; i++) {
                values[i] /= other.values[i];
            }

            return *this;
        }

        TVecN& operator+=(T a) {
            for (int i = 0; i < N; i++) {
                values[i] += a;
            }

            return *this;
        }

        TVecN& operator-=(T a) {
            for (int i = 0; i < N; i++) {
                values[i] -= a;
            }

            return *this;
        }

        TVecN& operator*=(T a) {
            for (int i = 0; i < N; i++) {
                values[i] *= a;
            }

            return *this;
        }

        TVecN& operator/=(T a) {
            for (int i = 0; i < N; i++) {
                values[i] /= a;
            }

            return *this;
        }

        TVecN operator-() const {
            return *this * (-1);
        }

        bool operator<=(const TVecN& other) const {
            bool done = true;

            for (int i = 0; i < N; i++) {
                done = done && (values[i] <= other.values[i]);
            }
            return done;
        }

        bool operator>=(const TVecN& other) const {
            bool done = true;

            for (int i = 0; i < N; i++) {
                done = done && (values[i] >= other.values[i]);
            }
            return done;
        }

        bool operator<(const TVecN& other) const {
            bool done = true;

            for (int i = 0; i < N; i++) {
                done = done && (values[i] < other.values[i]);
            }
            return done;
        }

        bool operator>(const TVecN& other) const {
            bool done = true;

            for (int i = 0; i < N; i++) {
                done = done && (values[i] > other.values[i]);
            }
            return done;
        }

        bool operator==(const TVecN& other) const {
            bool r = true;

            for (int i = 0; i < N; i++) {
                r = r && (values[i] == other.values[i]);
            }

            return r;
        }

        bool operator!=(const TVecN& other) const {
            bool r = false;

            for (int i = 0; i < N; i++) {
                r = r || (values[i] != other.values[i]);
            }

            return r;
        }

        static T distance2(const TVecN& a, const TVecN& b) {
            return (a - b).length2();
        }

        static T distance(const TVecN& a, const TVecN& b) {
            return (a - b).length();
        }

        static T dot(const TVecN& a, const TVecN& b) {
            T r = 0;

            for (int i = 0; i < N; i++) {
                r += a.values[i] * b.values[i];
            }

            return r;
        }

        static T angle(const TVecN& a, const TVecN& b) {
            return Math::acos(dot(a.normalized(), b.normalized()));
        }

        static TVecN project(const TVecN& axis, const TVecN& v) {
            return axis * dot(axis, v);
        }

        /**
         * Vector cross product
         * 
         * @warning Defined only fir 3-dim vectors
         * 
         * @param a Input vector
         * @param b Input vector
         *
         * @return Vector c, which satisfies: (a, b, c) - its right three
         */
        static TVecN cross(const TVecN& a, const TVecN& b) {
            if constexpr (N == 3) {
                TVecN<T, 3> result;

                // a x b = det | i  j  k  |
                //             | x1 y1 z1 |
                //             | x2 y2 z2 |

                result[0] = a.y() * b.z() - a.z() * b.y();
                result[1] = a.z() * b.x() - a.x() * b.z();
                result[2] = a.x() * b.y() - a.y() * b.x();

                return result;
            } else {
                return TVecN();
            }
        }

        /**
         * Triple vector product - volume of the a figure with sign:
         * 
         * Positive volume if (a,b,c) - right-system three
         * 
         * @param a Input vector
         * @param b Input vector
         * @param c Input vector
         *
         * @return Signed volume
         */
        static T triple(const TVecN& a, const TVecN& b, const TVecN& c) {
            // a b c - tight three
            // a x b - area of figure
            // dot(a x b, c) - volume of the figure

            return dot(cross(a, b), c);
        }

        static TVecN lerp(float t, const TVecN& a, const TVecN& b) {
            TVecN r;

            for (int i = 0; i < N; i++) {
                r.values[i] = Math::lerp(t, a.values[i], b.values[i]);
            }

            return r;
        }

        static TVecN slerp(T t, T ang, const TVecN& a, const TVecN& b) {
            if (ang <= Math::THRESH_FLOAT32) {
                return lerp(t, a, b);
            }

            TVecN<T, N> r;
            const T     angle_sin = Math::sin(ang);
            const T     angle1    = Math::sin(ang * (1 - t)) / angle_sin;
            const T     angle2    = Math::sin(ang * t) / angle_sin;

            for (int i = 0; i < N; i++) {
                r.values[i] = a.values[i] * angle1 + b.values[i] * angle2;
            }

            return r;
        }

        static TVecN min(const TVecN& a, const TVecN& b) {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = Math::min(a.values[i], b.values[i]);
            }

            return r;
        }

        static TVecN max(const TVecN& a, const TVecN& b) {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = Math::max(a.values[i], b.values[i]);
            }

            return r;
        }

        static TVecN clamp(const TVecN& t, const TVecN& left, const TVecN& right) {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = Math::clamp(t.values[i], left.values[i], right.values[i]);
            }

            return r;
        }

        TVecN abs() const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = Math::abs(values[i]);
            }

            return r;
        }

        TVecN pow(T factor) const {
            TVecN<T, N> r;

            for (int i = 0; i < N; i++) {
                r.values[i] = Math::pow(values[i], factor);
            }

            return r;
        }

        T length2() const {
            T r = 0;

            for (int i = 0; i < N; i++) {
                r += values[i] * values[i];
            }

            return r;
        }

        T length() const {
            T l2 = length2();
            return Math::sqrt(l2);
        }

        TVecN normalized() const {
            T len2 = length2();

            if (len2 <= Math::THRESH_ZERO_NORM_SQUARED) {
                return TVecN();
            } else {
                TVecN r   = *this;
                T     len = Math::sqrt(len2);
                r *= static_cast<T>(1.0) / len;
                return r;
            }
        }

        [[nodiscard]] std::size_t hash() const {
            std::size_t h = 0;

            for (const auto& v : values) {
                h = h ^ std::hash<T>()(v);
            }

            return h;
        }

        T x() const {
            static_assert(N >= 1, "No component");
            return values[0];
        }

        T y() const {
            static_assert(N >= 2, "No component");
            return values[1];
        }

        T z() const {
            static_assert(N >= 3, "No component");
            return values[2];
        }

        T w() const {
            static_assert(N >= 4, "No component");
            return values[3];
        }

        void set_x(T v) {
            static_assert(N >= 1, "No component");
            values[0] = v;
        }

        void set_y(T v) {
            static_assert(N >= 2, "No component");
            values[1] = v;
        }

        void set_z(T v) {
            static_assert(N >= 3, "No component");
            values[2] = v;
        }

        void set_w(T v) {
            static_assert(N >= 4, "No component");
            values[3] = v;
        }

        std::string to_string() const {
            std::stringstream s;
            s << "(";
            s << values[0];
            for (int i = 1; i < N; i++) {
                s << "," << values[i];
            }
            s << ")";
            return s.str();
        }

        T&       operator[](int index) { return values[index]; }
        const T& operator[](int index) const { return values[index]; }

        T*       data() { return values; }
        const T* data() const { return values; }

        static TVecN axis_x() {
            return TVecN<T, N>(1);
        }

        static TVecN axis_y() {
            return TVecN<T, N>(0, 1);
        }

        static TVecN axis_z() {
            return TVecN<T, N>(0, 0, 1);
        }

    public:
        T values[N] = {};
    };

    using Vec2f = TVecN<float, 2>;
    using Vec3f = TVecN<float, 3>;
    using Vec4f = TVecN<float, 4>;

    using Vec2i = TVecN<int, 2>;
    using Vec3i = TVecN<int, 3>;
    using Vec4i = TVecN<int, 4>;

    using Vec2u = TVecN<unsigned int, 2>;
    using Vec3u = TVecN<unsigned int, 3>;
    using Vec4u = TVecN<unsigned int, 4>;

    using Vec2b = TVecN<bool, 2>;
    using Vec3b = TVecN<bool, 3>;
    using Vec4b = TVecN<bool, 4>;

    using Color3f = Vec3f;
    using Color4f = Vec4f;

    using Point2i = TVecN<int, 2>;
    using Point2f = TVecN<float, 2>;

    using Size2i = TVecN<int, 2>;
    using Size2f = TVecN<float, 2>;

    using Rect2i = Vec4i;

    template<typename T, int N>
    inline std::ostream& operator<<(std::ostream& ostream, const TVecN<T, N>& vec) {
        ostream << "(";
        ostream << vec[0];
        for (int i = 1; i < N; i++) {
            ostream << "," << vec[i];
        }
        ostream << ")";
        return ostream;
    }

    template<typename T, int N>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, TVecN<T, N>& v) {
        std::string str;
        WG_YAML_READ(context, node, str);

        std::stringstream stream(str);

        for (int i = 0; i < N; i++) {
            stream >> v[i];
        }

        return StatusCode::Ok;
    }

    template<typename T, int N>
    Status yaml_write(IoContext& context, YamlNodeRef node, const TVecN<T, N>& v) {
        std::stringstream stream;

        for (int i = 0; i < N; i++) {
            stream << v[i];
            if (i + 1 < N) stream << " ";
        }

        return yaml_write(context, node, stream.str());
    }

    template<typename T, int N>
    Status archive_read(IoContext& context, Archive& archive, TVecN<T, N>& v) {
        return archive.nread(sizeof(TVecN<T, N>), &v);
    }

    template<typename T, int N>
    Status archive_write(IoContext& context, Archive& archive, const TVecN<T, N>& v) {
        return archive.nwrite(sizeof(TVecN<T, N>), &v);
    }

}// namespace wmoge

namespace std {

    template<typename T, int N>
    struct hash<wmoge::TVecN<T, N>> {
    public:
        std::size_t operator()(const wmoge::TVecN<T, N>& vec) const {
            return vec.hash();
        }
    };

}// namespace std
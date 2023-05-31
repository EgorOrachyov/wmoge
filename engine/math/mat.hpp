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

#ifndef WMOGE_MAT_HPP
#define WMOGE_MAT_HPP

#include <array>
#include <cassert>
#include <sstream>
#include <string>

#include "math_utils.hpp"
#include "vec.hpp"

namespace wmoge {

    template<typename T, int M, int N>
    class TMatMxN {
    public:
        TMatMxN() noexcept = default;

        explicit TMatMxN(const std::array<TVecN<T, N>, M>& rows) noexcept {
            int row = 0;

            for (auto& r : rows) {
                for (int j = 0; j < N; j++) {
                    values[row * N + j] = r[j];
                }
            }
        }

        TMatMxN(const T* vs, int count) {
            assert(count == size());
            for (int i = 0; i < count; i++) {
                values[i] = vs[i];
            }
        }

        TMatMxN(T m_00, T m_01,
                T m_10, T m_11) noexcept : TMatMxN() {
            static_assert(M >= 2 && N >= 2, "Cannot apply to this type");

            values[0 * N + 0] = m_00;
            values[0 * N + 1] = m_01;
            values[1 * N + 0] = m_10;
            values[1 * N + 1] = m_11;
        }

        TMatMxN(T m_00, T m_01, T m_02,
                T m_10, T m_11, T m_12,
                T m_20, T m_21, T m_22) noexcept : TMatMxN() {
            static_assert(M >= 3 && N >= 3, "Cannot apply to this type");

            values[0 * N + 0] = m_00;
            values[0 * N + 1] = m_01;
            values[0 * N + 2] = m_02;

            values[1 * N + 0] = m_10;
            values[1 * N + 1] = m_11;
            values[1 * N + 2] = m_12;

            values[2 * N + 0] = m_20;
            values[2 * N + 1] = m_21;
            values[2 * N + 2] = m_22;
        }

        TMatMxN(T m_00, T m_01, T m_02, T m_03,
                T m_10, T m_11, T m_12, T m_13,
                T m_20, T m_21, T m_22, T m_23,
                T m_30, T m_31, T m_32, T m_33) noexcept : TMatMxN() {
            static_assert(M >= 4 && N >= 4, "Cannot apply to this type");

            values[0 * N + 0] = m_00;
            values[0 * N + 1] = m_01;
            values[0 * N + 2] = m_02;
            values[0 * N + 3] = m_03;

            values[1 * N + 0] = m_10;
            values[1 * N + 1] = m_11;
            values[1 * N + 2] = m_12;
            values[1 * N + 3] = m_13;

            values[2 * N + 0] = m_20;
            values[2 * N + 1] = m_21;
            values[2 * N + 2] = m_22;
            values[2 * N + 3] = m_23;

            values[3 * N + 0] = m_30;
            values[3 * N + 1] = m_31;
            values[3 * N + 2] = m_32;
            values[3 * N + 3] = m_33;
        }

        TMatMxN(const TMatMxN& other) noexcept {
            for (int i = 0; i < M * N; i++) {
                values[i] = other.values[i];
            }
        }

        TMatMxN(TMatMxN&& other) noexcept {
            for (int i = 0; i < M * N; i++) {
                values[i] = other.values[i];
            }
        }

        template<int M1, int N1>
        TMatMxN(const TMatMxN<T, M1, N1>& other) : TMatMxN() {
            static_assert(M1 <= M, "Matrix too large");
            static_assert(N1 <= N, "Matrix too large");

            for (int i = 0; i < M1; i++) {
                for (int j = 0; j < N1; j++) {
                    values[i * N + j] = other.values[i * N1 + j];
                }
            }

            for (int i = M1; i < M; i++) {
                values[i * N + i] = 1;
            }
        }

        TMatMxN& operator+=(const TMatMxN& other) {
            for (int i = 0; i < M * N; i++) {
                values[i] += other.values[i];
            }
            return *this;
        }

        TMatMxN& operator-=(const TMatMxN& other) {
            for (int i = 0; i < M * N; i++) {
                values[i] -= other.values[i];
            }
            return *this;
        }

        TMatMxN operator+(const TMatMxN& other) const {
            TMatMxN result = *this;
            result += other;
            return result;
        }

        TMatMxN operator-(const TMatMxN& other) const {
            TMatMxN result = *this;
            result -= other;
            return result;
        }

        template<int K>
        TMatMxN<T, M, K> operator*(const TMatMxN<T, N, K>& other) const {
            TMatMxN<T, M, K> result;

            for (int i = 0; i < M; i++) {
                for (int j = 0; j < K; j++) {
                    T v = 0;

                    for (int f = 0; f < N; f++) {
                        v += values[i * N + f] * other.values[f * K + j];
                    }

                    result.values[i * K + j] = v;
                }
            }

            return result;
        }

        TMatMxN& operator*=(T other) {
            for (int i = 0; i < M * N; i++) {
                values[i] *= other;
            }
            return *this;
        }

        TMatMxN& operator/=(T other) {
            for (int i = 0; i < M * N; i++) {
                values[i] /= other;
            }
            return *this;
        }

        TMatMxN& operator+=(T other) {
            for (int i = 0; i < M * N; i++) {
                values[i] += other;
            }
            return *this;
        }

        TMatMxN& operator-=(T other) {
            for (int i = 0; i < M * N; i++) {
                values[i] -= other;
            }
            return *this;
        }

        TMatMxN operator*(T value) const {
            TMatMxN result = *this;
            result *= value;
            return result;
        }

        TMatMxN operator/(T value) const {
            TMatMxN result = *this;
            result /= value;
            return result;
        }

        TMatMxN operator+(T value) const {
            TMatMxN result = *this;
            result += value;
            return result;
        }

        TMatMxN operator-(T value) const {
            TMatMxN result = *this;
            result -= value;
            return result;
        }

        TMatMxN& operator=(const TMatMxN& other) {
            if (this != &other) {
                for (int i = 0; i < M * N; i++) {
                    values[i] = other.values[i];
                }
            }
            return *this;
        }

        template<int M1, int N1>
        TMatMxN<T, M1, N1> sum_matrix(int i, int j) const {
            if (i + M1 > M) return {};
            if (j + N1 > N) return {};

            TMatMxN<T, M1, N1> result;

            for (int si = 0; si < M1; si++) {
                const T* row = &values[(si + i) * N + j];
                for (int sj = 0; sj < N1; sj++) {
                    result.values[si * N1 + sj] = *row;
                    row += 1;
                }
            }

            return result;
        }

        template<int M1, int N1>
        void sum_matrix(TMatMxN<T, M1, N1>& result, int i, int j) const {
            if (i + M1 > M) return;
            if (j + N1 > N) return;

            for (int si = 0; si < M1; si++) {
                const T* row = &values[(si + i) * N + j];
                for (int sj = 0; sj < N1; sj++) {
                    result.values[si * N1 + sj] = *row;
                    row += 1;
                }
            }
        }

        TMatMxN<T, M, N - 1> exclude_col(int columnIndex) const {
            TMatMxN<T, M, N - 1> result;

            for (int i = 0; i < M; i++) {
                for (int j = 0; j < columnIndex; j++) {
                    result.values[i * result.dim_n() + j] = values[i * N + j];
                }

                for (int j = columnIndex; j < result.dim_n(); j++) {
                    result.values[i * result.dim_n() + j] = values[i * N + j + 1];
                }
            }

            return result;
        }

        TMatMxN<T, N, M> transpose() const {
            TMatMxN<T, N, M> result;

            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    result.values[j * M + i] = values[i * N + j];
                }
            }

            return result;
        }

        TVecN<T, M> col(int index) const {
            assert(index < N);

            TVecN<T, M> result;
            for (int i = 0; i < M; i++) {
                result.values[i] = values[i * N + index];
            }

            return result;
        }

        TVecN<T, N> row(int index) const {
            assert(index < M);

            TVecN<T, N> result;
            for (int i = 0; i < N; i++) {
                result.values[i] = values[index * N + i];
            }

            return result;
        }

        TVecN<T, M> operator*(const TVecN<T, N>& v) const {
            TVecN<T, M> result;

            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    result.values[i] += values[i * N + j] * v[j];
                }
            }

            return result;
        }

        T*       operator[](int i) { return &values[i * N]; }
        const T* operator[](int i) const { return &values[i * N]; }

        T*       data() { return values; }
        const T* data() const { return values; }

        std::string to_string() const {
            std::stringstream s;
            s << "(";
            for (int i = 0; i < dim_m(); i++) {
                s << "row[" << i << "]=(";
                for (int j = 0; j < dim_n(); j++) {
                    s << values[i * N + j];
                    if (j + 1 < dim_n()) s << ",";
                }
                s << ")";
                if (i + 1 < dim_m()) s << ",";
            }
            s << ")";
            return s.str();
        }

        constexpr static int stride() { return N * sizeof(T); }
        constexpr static int size() { return N * M; }

        constexpr static int dim_m() { return M; }
        constexpr static int dim_n() { return N; }

    public:
        T values[M * N] = {};
    };

    using Mat2x2f = TMatMxN<float, 2, 2>;
    using Mat3x3f = TMatMxN<float, 3, 3>;
    using Mat2x4f = TMatMxN<float, 2, 4>;
    using Mat3x4f = TMatMxN<float, 3, 4>;
    using Mat4x4f = TMatMxN<float, 4, 4>;

    /**
     * @class TDetMxN
     * @brief Matrix det evaluation helper
     *
     * @tparam T Type of matrix values
     * @tparam DM Number of rows
     * @tparam DN Number of columns
     */
    template<typename T, int DM, int DN>
    struct TDetMxN;

    template<typename T>
    struct TDetMxN<T, 1, 1> {
        T operator()(const TMatMxN<T, 1, 1>& m) const {
            return m.values[0];
        }
    };

    template<typename T>
    struct TDetMxN<T, 2, 2> {
        T operator()(const TMatMxN<T, 2, 2>& m) const {
            return m.values[0] * m.values[3] - m.values[1] * m.values[2];
        }
    };

    template<typename T, int DN>
    struct TDetMxN<T, DN, DN> {
        T operator()(const TMatMxN<T, DN, DN>& m) const {
            T                      result = 0;
            TMatMxN<T, DN - 1, DN> sub;
            m.sum_matrix(sub, 1, 0);

            for (int i = 0; i < DN; i++) {
                result += (i % 2 ? -1 : 1) * m.values[i] * sub.exclude_col(i).Det();
            }

            return result;
        }
    };

    template<typename T, int DM, int DN>
    struct TDetMxN {
        T operator()(const TMatMxN<T, DM, DN>& m) const {
            return 0;
        }
    };

}// namespace wmoge

#endif//WMOGE_MAT_HPP

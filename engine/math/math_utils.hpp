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

#ifndef WMOGE_MATH_UTILS_HPP
#define WMOGE_MATH_UTILS_HPP

#include <cmath>

namespace wmoge {

    class Math {
    public:
        static const float BIG_NUMBER_FLOAT32;
        static const float SMALL_NUMBER_FLOAT32;

        /* Float calculations mistake */
        static const float THRESH_FLOAT32;
        static const float THRESH_POINT_ON_PLANE;
        static const float THRESH_POINTS_ARE_SAME;
        static const float THRESH_ZERO_NORM_SQUARED;
        static const float THRESH_COMPARE_FLOAT32;
        static const float THRESH_COMPARE_FLOAT64;

        /* (from c math lib) */
        static const double PI;
        static const double HALF_PI;
        static const double QUARTER_PI;
        static const double SQRT2;
        static const double E;

        /* (from c math lib) */
        static const float PIf;
        static const float HALF_PIf;
        static const float QUARTER_PIf;
        static const float SQRT2f;
        static const float Ef;

        static std::size_t round_to_pow2(std::size_t x) {
            std::size_t r = 1u;
            while (x > r) r *= 2u;
            return r;
        }

        static int div_up(int count, int step) {
            return (count + step - 1) / step;
        }

        static std::size_t align(std::size_t size, std::size_t alignment) {
            return alignment * ((size + alignment - 1) / alignment);
        }

        static void split(float v, float& integer, float& fractional) {
            fractional = std::modf(v, &integer);
        }

        static void split(double v, double& integer, double& fractional) {
            fractional = std::modf(v, &integer);
        }

        static float  min(float a, float b) { return std::fminf(a, b); }
        static double min(double a, double b) { return std::fmin(a, b); }

        static float  max(float a, float b) { return std::fmaxf(a, b); }
        static double max(double a, double b) { return std::fmax(a, b); }

        static int    sin(int a) { return int(std::sin(float(a))); }
        static float  sin(float a) { return std::sin(a); }
        static double sin(double a) { return std::sin(a); }

        static float  cos(float a) { return std::cos(a); }
        static double cos(double a) { return std::cos(a); }

        static float  tan(float a) { return std::tan(a); }
        static double tan(double a) { return std::tan(a); }

        static float  asin(float a) { return std::asin(a); }
        static double asin(double a) { return std::asin(a); }

        static int    acos(int a) { return int(std::acos(float(a))); }
        static float  acos(float a) { return std::acos(a); }
        static double acos(double a) { return std::acos(a); }

        static float  atan(float a) { return std::atan(a); }
        static double atan(double a) { return std::atan(a); }

        static float  atan2(float y, float x) { return std::atan2(y, x); }
        static double atan2(double y, double x) { return std::atan2(y, x); }

        static int    sqrt(int a) { return int(std::sqrt(float(a))); }
        static float  sqrt(float a) { return std::sqrt(a); }
        static double sqrt(double a) { return std::sqrt(a); }

        static float  exp(float a) { return std::exp(a); }
        static double exp(double a) { return std::exp(a); }

        static int    pow(int a, int b) { return int(std::pow(float(a), float(b))); }
        static float  pow(float a, float p) { return std::pow(a, p); }
        static double pow(double a, double p) { return std::pow(a, p); }

        static float  log(float a) { return std::log(a); }
        static double log(double a) { return std::log(a); }

        static float  log2(float a) { return std::log2(a); }
        static double log2(double a) { return std::log2(a); }

        static float  log10(float a) { return std::log10(a); }
        static double log10(double a) { return std::log10(a); }

        static float  abs(float a) { return std::fabs(a); }
        static double abs(double a) { return std::abs(a); }

        static float  deg_to_rad(float a) { return a / 180.0f * PIf; }
        static double deg_to_rad(double a) { return a / 180.0 * PI; }

        static float  rad_to_deg(float a) { return a * 180.0f / PIf; }
        static double rad_to_deg(double a) { return a * 180.0 / PI; }

        /** @return Math round to nearest int type  */
        static float round(float a) { return std::round(a); }
        /** @return Math round to nearest int type  */
        static double round(double a) { return std::round(a); }

        /** @return Math up round to int type  */
        static float ceil(float a) { return std::ceil(a); }
        /** @return Math up round to int type  */
        static double ceil(double a) { return std::ceil(a); }

        /** @return Math down round to int type  */
        static float floor(float a) { return std::floor(a); }
        /** @return Math down round to int type  */
        static double floor(double a) { return std::floor(a); }

        static bool same(float a, float b) {
            return abs(a - b) < THRESH_COMPARE_FLOAT32;
        }

        static bool same(double a, double b) {
            return abs(a - b) < THRESH_COMPARE_FLOAT64;
        }

        static float lerp(float t, float left, float right) {
            return left * (1.0f - t) + right * t;
        }

        static double lerp(double t, double left, double right) {
            return left * (1.0 - t) + right * t;
        }

        static float smooth_step(float t, float left, float right) {
            t = clamp((t - left) / (right - left), 0.0f, 1.0f);
            return 2.0f * t * t * (1.5f - t);
        }

        static double smooth_step(double t, double left, double right) {
            t = clamp((t - left) / (right - left), 0.0, 1.0);
            return 2.0 * t * t * (1.5 - t);
        }

        static float smoother_step(float t, float left, float right) {
            t = clamp((t - left) / (right - left), 0.0f, 1.0f);
            return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
        }

        static double smoother_step(double t, double left, double right) {
            t = clamp((t - left) / (right - left), 0.0, 1.0);
            return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
        }

        template<typename T>
        static bool between(T t, T left, T right) { return (t >= left) && (t <= right); }

        template<typename T>
        static T clamp(T t, T left, T right) { return (t < left ? left : (t > right ? right : t)); }

        template<typename T>
        static T min(T a, T b) { return (a < b ? a : b); }

        template<typename T>
        static T max(T a, T b) { return (a > b ? a : b); }

        template<typename T>
        static T abs(T a) { return (a > 0 ? a : -a); }

        template<typename T, T a>
        static constexpr T const_max() noexcept { return a; };

        template<typename T, T a, T b, T... args>
        static constexpr T const_max() noexcept {
            return (a > b ? const_max<T, a, args...>() : const_max<T, b, args...>());
        };

        template<typename T, T a>
        static constexpr T const_min() noexcept { return a; };

        template<typename T, T a, T b, T... args>
        static constexpr T const_min() noexcept {
            return (a < b ? const_min<T, a, args...>() : const_min<T, b, args...>());
        };
    };

}// namespace wmoge

#endif//WMOGE_MATH_UTILS_HPP

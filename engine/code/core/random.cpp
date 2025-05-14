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

#include "random.hpp"

#include <ctime>
#include <thread>

namespace wmoge {

    static std::default_random_engine& get_rnd_engine() {
        thread_local std::default_random_engine engine(
                static_cast<unsigned int>(std::time(nullptr)) +
                static_cast<unsigned int>(std::hash<std::thread::id>{}(std::this_thread::get_id())));

        return engine;
    }

    std::uint64_t Random::next_uint64() {
        std::uniform_int_distribution<std::uint64_t> dist;
        return dist(get_rnd_engine());
    }
    std::uint64_t Random::next_uint64(std::uint64_t left, std::uint64_t right) {
        std::uniform_int_distribution<std::uint64_t> dist(left, right);
        return dist(get_rnd_engine());
    }

    float Random::next_float() {
        std::uniform_real_distribution<float> dist;
        return dist(get_rnd_engine());
    }
    float Random::next_float(float left, float right) {
        std::uniform_real_distribution<float> dist(left, right);
        return dist(get_rnd_engine());
    }

}// namespace wmoge

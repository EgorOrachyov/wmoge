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

#include <cinttypes>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class StringUtils
     * @brief Utils to work with default string class
     */
    class StringUtils {
    public:
        /** @return Convert string content to value */
        static float to_float(const std::string& str);

        /** @return Convert string content to value */
        static double to_double(const std::string& str);

        /** @return Convert string content to value */
        static int to_int(const std::string& str, int base = 10);

        /** @return Convert string content to value */
        static unsigned long to_ulong(const std::string& str, int base = 10);

        /** @return Convert string content to value */
        static std::uint64_t to_uint64(const std::string& str, int base = 10);

        /** @return Converted int value to string */
        static std::string from_int(int value);

        /** @return Converted uint value to string */
        static std::string from_uint(unsigned int value);

        /** @return Converted uint64 value to string */
        static std::string from_uint64(std::uint64_t value);

        /** @return Converted float value to string */
        static std::string from_float(float value, int precision = 5);

        /** @return Converted double value to string */
        static std::string from_double(double value, int precision = 5);

        /** @return Converted bool value to string */
        static std::string from_bool(bool value);

        /** @return Converted ptr value to string */
        static std::string from_ptr(const void* value);

        /** @return Converted mem size value to pretty string */
        static std::string from_mem_size(std::size_t bytes_size);

        /** @return Split string by symbols */
        static void split(const std::string& what, const std::string& splitters, std::vector<std::string>& result);

        /** @return Finds and replaces first matched substring in a source string */
        static std::string find_replace_first(const std::string& source, const std::string& substring, const std::string& replacement);
    };

}// namespace wmoge
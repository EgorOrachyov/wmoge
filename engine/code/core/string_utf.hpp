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

#include <string>

namespace wmoge {

    /**
     * @class StringUtf
     * @brief Utility to work with utf encoding for strings
     */
    class StringUtf {
    public:
        using Char32u = char32_t;
        using Char16u = char16_t;
        using Char8u  = char;

        /** Convert code point to lower case */
        static Char32u to_lower_chr(Char32u ch);

        /** Convert code point to lower case */
        static Char32u to_upper_chr(Char32u ch);

        /** Convert utf-32 point to utf-8 */
        static bool utf32_to_utf8_chr(Char32u ch, Char8u* out, int& len);

        /** Convert utf-8 point to utf-32 */
        static bool utf8_to_utf32_chr(const Char8u* in, int& len, Char32u& out);

        /** Convert utf-32 point to utf-16 point */
        static bool utf32_to_utf16_chr(Char32u ch, Char16u* out, int& outLen);

        /** Convert utf-16 point to utf-32 point */
        static bool utf16_to_utf32_chr(const Char16u* in, int& len, Char32u& out);

        /**
         * @brief Convert utf-8 encoded string to utf-16 encoded string
         *
         * @param in String to convert
         * @param[out] out String to store result
         *
         * @return True if successfully converted
         */
        static bool utf8_to_utf16(const std::string& in, std::u16string& out);

        /**
         * @brief Convert utf-16 encoded string to utf-8 encoded string
         *
         * @param in String to convert
         * @param[out] out String to store result
         *
         * @return True if successfully converted
         */
        static bool utf16_to_utf8(const std::u16string& in, std::string& out);
    };

}// namespace wmoge
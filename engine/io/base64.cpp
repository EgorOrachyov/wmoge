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

#include "base64.hpp"

#include <base64.h>
#include <cassert>
#include <cstring>

namespace wmoge {

    bool Base64::encode(const std::uint8_t* data, int bytesCount, std::string& result) {
        result = base64_encode(static_cast<const unsigned char*>(data), bytesCount);
        return true;
    }
    bool Base64::encode(const std::vector<std::uint8_t>& data, std::string& result) {
        return encode(data.data(), int(data.size()), result);
    }
    bool Base64::encode(const Ref<Data>& data, std::string& result) {
        assert(data);
        return encode(data->buffer(), int(data->size()), result);
    }

    bool Base64::decode(const std::string_view& data, std::vector<std::uint8_t>& result) {
        std::string decoded = base64_decode(data);

        if (!decoded.empty()) {
            result.resize(decoded.length() * sizeof(char));
            std::memcpy(result.data(), decoded.c_str(), decoded.length() * sizeof(char));
        }

        return true;
    }
    bool Base64::decode(const std::string_view& data, Ref<Data>& result) {
        std::string decoded = base64_decode(data);

        if (!decoded.empty()) {
            result = make_ref<Data>(decoded.c_str(), decoded.length() * sizeof(char));
        }

        return true;
    }
    bool Base64::decode(const std::string_view& data, std::string& result) {
        result = base64_decode(data);
        return true;
    }

}// namespace wmoge
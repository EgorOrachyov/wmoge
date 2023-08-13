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

#include "archive.hpp"

namespace wmoge {

    Status Archive::write(bool value) {
        assert(can_write());
        return write(int(value));
    }
    Status Archive::write(int value) {
        assert(can_write());
        return nwrite(sizeof(value), &value);
    }
    Status Archive::write(float value) {
        assert(can_write());
        return nwrite(sizeof(value), &value);
    }
    Status Archive::write(std::size_t value) {
        assert(can_write());
        return nwrite(sizeof(value), &value);
    }
    Status Archive::write(const StringId& value) {
        assert(can_write());
        return write(value.str());
    }
    Status Archive::write(const std::string& value) {
        assert(can_write());
        int len = static_cast<int>(value.size());
        write(len);
        nwrite(len, value.data());
        return StatusCode::Ok;
    }
    Status Archive::read(bool& value) {
        assert(can_read());
        int v;
        read(v);
        value = v;
        return StatusCode::Ok;
    }
    Status Archive::read(int& value) {
        assert(can_read());
        return nread(sizeof(value), &value);
    }
    Status Archive::read(float& value) {
        assert(can_read());
        return nread(sizeof(value), &value);
    }
    Status Archive::read(std::size_t& value) {
        assert(can_read());
        return nread(sizeof(value), &value);
    }
    Status Archive::read(StringId& value) {
        assert(can_read());
        std::string s;
        read(s);
        value = StringId(s);
        return StatusCode::Ok;
    }
    Status Archive::read(std::string& value) {
        assert(can_read());
        int len;
        read(len);
        value.resize(len);
        return nread(len, value.data());
    }

    Status archive_write(Archive& archive, const bool& value) {
        archive.write(value);
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const int& value) {
        archive.write(value);
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const float& value) {
        archive.write(value);
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const std::size_t& value) {
        archive.write(value);
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const StringId& value) {
        archive.write(value);
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const std::string& value) {
        archive.write(value);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, bool& value) {
        archive.read(value);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, int& value) {
        archive.read(value);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, float& value) {
        archive.read(value);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, std::size_t& value) {
        archive.read(value);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, StringId& value) {
        archive.read(value);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, std::string& value) {
        archive.read(value);
        return StatusCode::Ok;
    }

}// namespace wmoge
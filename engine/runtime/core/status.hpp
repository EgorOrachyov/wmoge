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

#include "core/string_id.hpp"
#include "io/enum.hpp"

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @brief List of engine errors
     */
    enum class StatusCode {
        Ok = 0,           // Success
        Error,            // General error code
        NotImplemented,   // Functionality not implemented
        FailedInstantiate,// Cannot instantiate object from class
        FailedLoadLibrary,// Failed to open dynamic library
        FailedLoadSymbol, // Failed to open dynamic symbol
        FailedOpenFile,   // Failed to open file or stream for read/write operations
        FailedFindFile,   // Failed to find file or stream
        FailedParse,      // Failed to parse structured file
        FailedRead,       // Failed to perform reading of some data
        FailedWrite,      // Failed to write some data
        FailedEncode,     // Failed to encode binary data
        FailedDecode,     // Failed to decode binary data
        FailedResize,     // Data resize error
        FailedCompress,   // Compression bc, lz, zip error
        FailedDecompress, // Decompression bc, lz, zip error
        FailedCompile,    // Shader compilation
        InvalidData,      // Passed invalid data to process
        InvalidParameter, // Function invalid parameter
        InvalidState,     // State of object is invalid for call
        NoProperty,       // No such property in object to get/set
        NoMethod,         // No such method in object to call
        NoClass,          // No registered class
        NoValue,          // No value to return
        NoAsset,          // No such asset in engine files
        ExitCode0,        // App should exit, code 0 - everything is Ok, normal close
        ExitCode1         // App should exit, code 1 - Error occured, normal close
    };

    /**
     * @class Status
     * @brief Wrapper for return status code and an optional error message instead of exception
     */
    class Status {
    public:
        Status() = default;

#ifdef WG_DEBUG
        Status(StatusCode code, std::string message = std::string{}) : m_code(code), m_message(std::move(message)) {}
#else
        Status(StatusCode code, std::string message = std::string{}) : m_code(code) {}
#endif

        Status(const Status&)     = default;
        Status(Status&&) noexcept = default;

        Status& operator=(const Status&)     = default;
        Status& operator=(Status&&) noexcept = default;

        bool operator==(const Status& other) const { return m_code == other.m_code; }
        bool operator!=(const Status& other) const { return m_code != other.m_code; }

        [[nodiscard]] bool is_ok() const { return m_code == StatusCode::Ok; }
        [[nodiscard]] bool is_error() const { return m_code != StatusCode::Ok; }

        operator bool() const { return is_ok(); }

        [[nodiscard]] StatusCode  code() const { return m_code; }
        [[nodiscard]] StatusCode& code() { return m_code; }

#ifdef WG_DEBUG
        [[nodiscard]] std::string message() const { return m_message; }
#else
        [[nodiscard]] std::string message() const { return ""; }
#endif

    private:
#ifdef WG_DEBUG
        std::string m_message;
#endif
        StatusCode m_code = StatusCode::Ok;
    };

    inline std::ostream& operator<<(std::ostream& stream, const Status& status) {
        stream << '\'' << Enum::to_str(status.code()) << '\'';
        return stream;
    }

#define WG_CHECKED(operation)               \
    {                                       \
        const Status _status = (operation); \
        if (!_status) {                     \
            return _status;                 \
        }                                   \
    }

#define WG_OK StatusCode::Ok

}// namespace wmoge
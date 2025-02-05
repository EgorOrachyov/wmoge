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

#include "core/status.hpp"

#include <cxxopts.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class CmdLineUtil
     * @brief Util for command-line
     */
    class CmdLineUtil {
    public:
        static std::string              to_string(int argc, const char* const* argv);
        static std::vector<std::string> to_vector(int argc, const char* const* argv);
    };

    /**
     * @class CmdLineParseResult
     * @brief Container for command-line parse result
     */
    class CmdLineParseResult {
    public:
        CmdLineParseResult(cxxopts::ParseResult parsed);

        [[nodiscard]] int         get_int(const std::string& name);
        [[nodiscard]] bool        get_bool(const std::string& name);
        [[nodiscard]] std::string get_string(const std::string& name);

    private:
        cxxopts::ParseResult m_parsed;
    };

    using CmdLineArgs = std::vector<std::string>;

    /**
     * @class CmdLineOptions
     * @brief Container for command-line options
     */
    class CmdLineOptions {
    public:
        CmdLineOptions(const std::string& name, const std::string& desc);

        void add_int(const std::string& name, const std::string& desc, const std::string& value = "");
        void add_bool(const std::string& name, const std::string& desc, const std::string& value = "");
        void add_string(const std::string& name, const std::string& desc, const std::string& value = "");

        [[nodiscard]] std::optional<CmdLineParseResult> parse(const std::vector<std::string>& args);
        [[nodiscard]] std::string                       get_help() const;

    private:
        cxxopts::Options m_options;
    };

    /** 
     * @brief Cmd line hook
     */
    using CmdLineHook = std::function<Status(CmdLineParseResult& cmd_line)>;

    /** 
     * @class CmdLineHookList
     * @brief Storage for cmd line hooks
     */
    class CmdLineHookList {
    public:
        CmdLineHookList() = default;

        void   add(CmdLineHook hook);
        void   clear();
        Status process(CmdLineParseResult& cmd_line);

    private:
        std::vector<CmdLineHook> m_storage;
    };

}// namespace wmoge
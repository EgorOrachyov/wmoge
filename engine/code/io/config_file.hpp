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

#include "core/flat_map.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "math/color.hpp"

#include <string>

namespace wmoge {

    /** @brief How to stack configs */
    enum class ConfigStackMode {
        Overwrite,
        Keep
    };

    /**
     * @class ConfigFile
     * @brief Ini-file based simple config file
     */
    class ConfigFile final : public RefCnt {
    public:
        ConfigFile() = default;

        /**
         * @brief Loads config file from a engine directory
         * @param file_system File system access
         * @param path Relative path to the file to load
         * @return True if successfully loaded and parsed
         */
        Status load_from_file(class FileSystem* file_system, const std::string& path);

        /**
         * @brief Loads config file from string with content
         * @param content Confing content
         * @return True if successfully loaded and parsed
         */
        Status load_from_content(const std::string& content);

        /**
         * @brief Stack other config on top of this
         *
         * @param other Config to stack
         * @param mode Mode to handle intersecting entries
         */
        Status stack(const ConfigFile& other, ConfigStackMode mode = ConfigStackMode::Overwrite);

        /** @brief Clears config file */
        void clear();

        /** @brief Check if config file has no entries */
        bool is_empty();

        bool get_value(const Strid& key, Var*& element);
        bool get_value(const Strid& key, const Var*& element) const;

        Status set_bool(const Strid& key, const bool& value, bool overwrite = true);
        Status set_int(const Strid& key, const int& value, bool overwrite = true);
        Status set_float(const Strid& key, const float& value, bool overwrite = true);
        Status set_string(const Strid& key, const std::string& value, bool overwrite = true);

        Status get_bool(const Strid& key, bool& value) const;
        Status get_int(const Strid& key, int& value) const;
        Status get_float(const Strid& key, float& value) const;
        Status get_string(const Strid& key, std::string& value) const;
        Status get_color4f(const Strid& key, Color4f& value) const;

        [[nodiscard]] bool        get_bool_or_default(const Strid& key, bool def_value = false) const;
        [[nodiscard]] int         get_int_or_default(const Strid& key, int def_value = 0) const;
        [[nodiscard]] float       get_float_or_default(const Strid& key, float def_value = 1.0f) const;
        [[nodiscard]] std::string get_string_or_default(const Strid& key, std::string def_value = "") const;
        [[nodiscard]] Color4f     get_color4f_or_default(const Strid& key, Color4f def_value = {}) const;

    private:
        flat_map<Strid, Var> m_entries;
    };

}// namespace wmoge
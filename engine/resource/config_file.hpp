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

#ifndef WMOGE_CONFIG_FILE_HPP
#define WMOGE_CONFIG_FILE_HPP

#include "core/fast_map.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "math/color.hpp"
#include "resource/resource.hpp"

#include <string>

namespace wmoge {

    /**
     * @class ConfigStackMode
     * @brief How to stack configs
     */
    enum class ConfigStackMode {
        Overwrite,
        Keep
    };

    /**
     * @class ConfigFile
     * @brief Ini-file based simple config file
     */
    class ConfigFile final : public Resource {
    public:
        WG_OBJECT(ConfigFile, Resource);

        /**
         * @brief Loads config file from a engine directory
         *
         * @param path Relative path to the file to load
         *
         * @return True if successfully loaded and parsed
         */
        Status load(const std::string& path);

        /**
         * @brief Stack other config on top of this
         *
         * @param other Config to stack
         * @param mode Mode to handle intersecting entries
         */
        Status stack(const ConfigFile& other, ConfigStackMode mode = ConfigStackMode::Overwrite);

        /**
         * @brief Loads config by path and stacks into this
         *
         * @param path Path to config to load and stack
         * @param mode Stacking mode
         *
         * @return True if loaded and stacked
         */
        Status load_and_stack(const std::string& path, ConfigStackMode mode = ConfigStackMode::Overwrite);

        /** @brief Clears config */
        void clear();

        /** @brief Check if config has no entries */
        bool is_empty();

        Status set(const StringId& key, const bool& value, bool overwrite = true);
        Status set(const StringId& key, const int& value, bool overwrite = true);
        Status set(const StringId& key, const float& value, bool overwrite = true);
        Status set(const StringId& key, const std::string& value, bool overwrite = true);

        Status get(const StringId& key, bool& value);
        Status get(const StringId& key, int& value);
        Status get(const StringId& key, float& value);
        Status get(const StringId& key, std::string& value);
        Status get(const StringId& key, Color4f& value);

        bool        get_bool(const StringId& key, bool def_value = false);
        int         get_int(const StringId& key, int def_value = 0);
        float       get_float(const StringId& key, float def_value = 1.0f);
        std::string get_string(const StringId& key, std::string def_value = "");
        Color4f     get_color4f(const StringId& key, Color4f value = {});

        Status copy_to(Object& other) const override;

    private:
        /** @brief Single element in a config file */
        bool get_element(const StringId& key, Var*& element);

    private:
        fast_map<StringId, Var> m_entries;
    };

}// namespace wmoge

#endif//WMOGE_CONFIG_FILE_HPP

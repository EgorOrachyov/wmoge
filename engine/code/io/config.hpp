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

#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "io/config_file.hpp"
#include "io/enum.hpp"

#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @class Config
     * @brief Global manager for confgiguration variable and config files
    */
    class Config {
    public:
        Config(class IocContainer* ioc);

        Status load(const std::string& path, ConfigStackMode mode = ConfigStackMode::Overwrite);

        Status set_bool(const Strid& key, const bool& value, bool overwrite = true);
        Status set_int(const Strid& key, const int& value, bool overwrite = true);
        Status set_float(const Strid& key, const float& value, bool overwrite = true);
        Status set_string(const Strid& key, const std::string& value, bool overwrite = true);

        Status get_bool(const Strid& key, bool& value) const;
        Status get_int(const Strid& key, int& value) const;
        Status get_float(const Strid& key, float& value) const;
        Status get_string(const Strid& key, std::string& value) const;
        Status get_color4f(const Strid& key, Color4f& value) const;

        Status try_get_value_of(const Strid& key, VarType type, Var& value);

        [[nodiscard]] bool        get_bool_or_default(const Strid& key, bool def_value = false) const;
        [[nodiscard]] int         get_int_or_default(const Strid& key, int def_value = 0) const;
        [[nodiscard]] float       get_float_or_default(const Strid& key, float def_value = 1.0f) const;
        [[nodiscard]] std::string get_string_or_default(const Strid& key, std::string def_value = "") const;
        [[nodiscard]] Color4f     get_color4f_or_default(const Strid& key, Color4f def_value = {}) const;

    private:
        Ref<ConfigFile>   m_file        = make_ref<ConfigFile>();
        class FileSystem* m_file_system = nullptr;
    };

    inline Status config_read(Config* config, const std::string& key, bool& value) { return config->get_bool(SID(key), value); }
    inline Status config_read(Config* config, const std::string& key, int& value) { return config->get_int(SID(key), value); }
    inline Status config_read(Config* config, const std::string& key, float& value) { return config->get_float(SID(key), value); }
    inline Status config_read(Config* config, const std::string& key, std::string& value) { return config->get_string(SID(key), value); }
    inline Status config_read(Config* config, const std::string& key, Color4f& value) { return config->get_color4f(SID(key), value); }

    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    inline Status config_read(Config* config, const std::string& key, T& value) {
        if (std::string str_value; config->get_string(SID(key), str_value)) {
            auto parsed = magic_enum::enum_cast<T>(str_value);
            if (!parsed.has_value()) {
                return StatusCode::FailedRead;
            }
            value = parsed.value();
        }
        return WG_OK;
    }

}// namespace wmoge

#define WG_CFG_READ(cfg, section, owner, variable) \
    config_read(cfg, section + "." + #variable, owner.variable)

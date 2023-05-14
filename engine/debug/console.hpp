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

#ifndef WMOGE_CONSOLE_HPP
#define WMOGE_CONSOLE_HPP

#include "core/fast_map.hpp"
#include "core/log.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "event/event_listener.hpp"
#include "math/color.hpp"
#include "resource/font.hpp"

#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class ConsoleObject
     * @brief Base class for any console object
     */
    class ConsoleObject {
    public:
        virtual ~ConsoleObject() = default;

        const StringId&    get_name() const { return m_name; }
        const std::string& get_help() const { return m_help; }

        virtual bool is_cmd() const { return false; }
        virtual bool is_var() const { return false; }

    protected:
        friend class Console;
        StringId    m_name;
        std::string m_help;
    };

    /**
     * @class ConsoleVar
     * @brief An external variable which can be registered and modified through console
     */
    class ConsoleVar final : public ConsoleObject {
    public:
        ~ConsoleVar() override = default;

        const Var&                                         get_value() const { return m_value; }
        const Var&                                         get_default() const { return m_default; }
        const std::function<void(const Var&, const Var&)>& get_on_changed() const { return m_on_changed; }
        bool                                               is_var() const override { return true; }

        void change(Var new_value);

        int         as_int() const;
        float       as_float() const;
        std::string as_string() const;

    private:
        friend class Console;
        Var                                         m_value;
        Var                                         m_default;
        std::function<void(const Var&, const Var&)> m_on_changed;
    };

    /**
     * @class ConsoleCmd
     * @brief An external command which can be executed from the console
     */
    class ConsoleCmd final : public ConsoleObject {
    public:
        ~ConsoleCmd() override = default;

        const std::function<int(const std::vector<std::string>& args)>& get_func() const { return m_func; }
        bool                                                            is_cmd() const override { return true; }

    private:
        friend class Console;
        std::function<int(const std::vector<std::string>& args)> m_func;
    };

    /**
     * @class Console
     * @brief In-game debug console to run commands and change variables
     */
    class Console final {
    public:
        /**
         * @brief Register new console variable
         *
         * @param name Unique name of the variable without spaces
         * @param default_value Initial (default) value of the variable
         * @param help Optional help text for the user
         * @param on_changed Optional callback to call before new value is set
         *
         * @return Registered variable on success
         */
        ConsoleVar* register_var(StringId name, Var default_value, std::string help, std::function<void(const Var& prev, const Var& next)> on_changed = {});
        /**
         * @brief Register new console command
         *
         * @param name Unique name of the command without spaces
         * @param help Optional help text for the user
         * @param function Function to execute for a command
         *
         * @return Registered command on success
         */
        ConsoleCmd* register_cmd(StringId name, std::string help, std::function<int(const std::vector<std::string>& args)> function);

        /** @brief Find existing console var by name */
        ConsoleVar* find_var(const StringId& name);
        /** @brief Find existing console cmd by name */
        ConsoleCmd* find_cmd(const StringId& name);

        /** @brief Add message to the console as a text */
        void add_info(const std::string& text);
        /** @brief Add message to the console as a warning */
        void add_warning(const std::string& text);
        /** @brief Add message to the console as an error */
        void add_error(const std::string& text);
        /** @brief Add message to the console as an suggestion */
        void add_suggestion(const std::string& text);

        /** @brief Process console input as a string */
        void process(const std::string& input);

        void init();
        void shutdown();
        void update();
        void render();
        void setup_log(LogLevel level);

    private:
        void register_commands();
        void load_settings();
        void add_message_internal(const std::string& message, const Color4f& color, bool merge_lines = false);

        enum class ConsoleState {
            Closed,
            Opening,
            Open,
            Closing
        };
        struct Message {
            std::string text;
            Color4f     color;
        };

    private:
        fast_map<StringId, ConsoleVar> m_vars;
        fast_map<StringId, ConsoleCmd> m_cmds;
        std::vector<Message>           m_messages;
        std::vector<std::string>       m_history;
        std::vector<std::string>       m_to_process;
        std::recursive_mutex           m_mutex;

        std::shared_ptr<class LogListenerConsole> m_log_listener;
        Ref<EventListener>                        m_listener_keyboard;

        Ref<Font> m_console_font;
        Color4f   m_color_back       = Color::from_hex4(0x000000ee);
        Color4f   m_color_line       = Color::from_hex4(0x060606be);
        Color4f   m_color_text       = Color::from_hex4(0xefefefff);
        Color4f   m_color_input      = Color::from_hex4(0x43fc3eff);
        Color4f   m_color_warning    = Color::from_hex4(0xffea00ff);
        Color4f   m_color_error      = Color::from_hex4(0xff0000ff);
        Color4f   m_color_suggestion = Color::from_hex4(0x0ff3ffff);
        Color4f   m_color_cursor     = Color::from_hex4(0xf0f0f0ff);
        float     m_speed_open       = 6.0f;
        float     m_speed_blink      = 1.0f;
        float     m_blink_threshold  = 0.6f;
        float     m_size             = 0.4f;
        float     m_text_size        = 14.0f;
        float     m_line_size        = 21.0f;
        float     m_cursor_width     = 7.0f;
        float     m_cursor_height    = 15.0f;
        float     m_text_line        = 7.0f;
        float     m_margin           = 3.0f;
        float     m_margin_line      = 14.0f;

        class AuxDrawCanvas* m_canvas = nullptr;
        std::string          m_line;
        ConsoleState         m_state           = ConsoleState::Closed;
        float                m_state_open      = 0.0f;
        float                m_state_blink     = 0.0f;
        float                m_current_speed   = 0.0f;
        float                m_cursor_offset   = 0.0f;
        int                  m_scroll_messages = 0;
        int                  m_max_to_display  = 0;
    };

}// namespace wmoge

#endif//WMOGE_CONSOLE_HPP

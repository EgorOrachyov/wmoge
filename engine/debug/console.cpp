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

#include "console.hpp"

#include "core/engine.hpp"
#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "debug/profiler.hpp"
#include "event/event.hpp"
#include "event/event_action.hpp"
#include "event/event_input.hpp"
#include "event/event_manager.hpp"
#include "math/math_utils.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"
#include "render/aux_draw_canvas.hpp"
#include "resource/config_file.hpp"
#include "resource/resource_manager.hpp"

#include <algorithm>
#include <sstream>

namespace wmoge {

    void ConsoleVar::change(Var new_value) {
        if (m_on_changed) m_on_changed(m_value, new_value);
        m_value = std::move(new_value);
        WG_LOG_INFO("change value " << m_name << " to " << m_value);
    }
    int ConsoleVar::as_int() const {
        return (int) m_value;
    }
    float ConsoleVar::as_float() const {
        return (float) m_value;
    }
    std::string ConsoleVar::as_string() const {
        return (std::string) m_value;
    }

    ConsoleVar* Console::register_var(StringId name, Var default_value, std::string help, std::function<void(const Var&, const Var&)> on_changed) {
        std::lock_guard lock(m_mutex);

        if (m_vars.find(name) != m_vars.end()) {
            WG_LOG_ERROR("an attempt to re-register var " << name);
            return nullptr;
        }

        auto& var        = m_vars[name];
        var.m_name       = std::move(name);
        var.m_help       = std::move(help);
        var.m_value      = default_value;
        var.m_default    = std::move(default_value);
        var.m_on_changed = std::move(on_changed);

        return &var;
    }
    ConsoleCmd* Console::register_cmd(StringId name, std::string help, std::function<int(const std::vector<std::string>&)> function) {
        std::lock_guard lock(m_mutex);

        if (m_cmds.find(name) != m_cmds.end()) {
            WG_LOG_ERROR("an attempt to re-register cmd " << name);
            return nullptr;
        }

        auto& cmd  = m_cmds[name];
        cmd.m_name = std::move(name);
        cmd.m_help = std::move(help);
        cmd.m_func = std::move(function);

        return &cmd;
    }
    ConsoleVar* Console::find_var(const StringId& name) {
        std::lock_guard lock(m_mutex);
        auto            query = m_vars.find(name);
        return query != m_vars.end() ? &query->second : nullptr;
    }
    ConsoleCmd* Console::find_cmd(const StringId& name) {
        std::lock_guard lock(m_mutex);
        auto            query = m_cmds.find(name);
        return query != m_cmds.end() ? &query->second : nullptr;
    }

    void Console::add_info(const std::string& text) {
        add_message_internal(text, m_color_text);
    }
    void Console::add_warning(const std::string& text) {
        add_message_internal(text, m_color_warning);
    }
    void Console::add_error(const std::string& text) {
        add_message_internal(text, m_color_error);
    }
    void Console::add_suggestion(const std::string& text) {
        add_message_internal(text, m_color_suggestion);
    }

    void Console::process(const std::string& input) {
        if (input.empty()) return;
        add_message_internal("> " + input, m_color_input);

        ConsoleCmd*              cmd;
        std::vector<std::string> args;
        StringUtils::split(input, " ", args);

        if (args.empty()) {
            add_error("failed to parse input");
            return;
        }
        cmd = find_cmd(SID(args[0]));
        if (!cmd) {
            add_error("no such cmd to execute " + args[0]);
            return;
        }
        if (int ret = cmd->get_func()(args)) {
            add_error("cmd finished with " + StringUtils::from_int(ret));
        }
    }

    void Console::init() {
        register_commands();
        load_settings();

        m_actions_listener = make_listener<EventAction>([this](const EventAction& event) {
            // Opening closing
            {
                std::lock_guard guard(m_mutex);

                if (event.name == SID("cn_trigger")) {
                    if (m_state == ConsoleState::Closed || m_state == ConsoleState::Closing) {
                        m_current_speed = m_speed_open;
                        m_state         = ConsoleState::Opening;
                        return true;
                    }
                    if (m_state == ConsoleState::Open || m_state == ConsoleState::Opening) {
                        m_current_speed = -m_speed_open;
                        m_state         = ConsoleState::Closing;
                        return true;
                    }
                }
            }

            std::string line_to_process;

            // Control
            {
                std::lock_guard guard(m_mutex);

                if (m_state == ConsoleState::Open) {
                    if (event.name == SID("cn_delete") && !m_line.empty()) {
                        m_line.pop_back();
                        m_cursor_offset = m_console_font->get_string_size(m_line, m_text_size).x();
                        return true;
                    } else if (event.name == SID("cn_submit") && !m_line.empty()) {
                        m_cursor_offset   = 0;
                        m_scroll_messages = 0;
                        std::swap(line_to_process, m_line);
                    } else if (event.name == SID("cn_scroll_up")) {
                        m_scroll_messages = Math::max(0, Math::min(m_scroll_messages + 1, static_cast<int>(m_messages.size()) - m_max_to_display));
                        return true;
                    } else if (event.name == SID("cn_scroll_down")) {
                        m_scroll_messages = Math::max(m_scroll_messages - 1, 0);
                        return true;
                    }
                }
            }

            if (!line_to_process.empty()) {
                process(line_to_process);
                return true;
            }

            return false;
        });

        m_keyboard_listener = make_listener<EventKeyboard>([this](const EventKeyboard& event) {
            // Input
            {
                std::lock_guard guard(m_mutex);

                if (m_state == ConsoleState::Open) {
                    if (event.action == InputAction::Text && !event.text.empty()) {
                        m_line += event.text;
                        m_cursor_offset = m_console_font->get_string_size(m_line, m_text_size).x();
                        return true;
                    }
                }
            }

            return false;
        });

        auto* event_manager = Engine::instance()->event_manager();
        event_manager->subscribe(m_actions_listener);
        event_manager->subscribe(m_keyboard_listener);
    }
    void Console::shutdown() {
        m_actions_listener.reset();
        m_keyboard_listener.reset();
        m_console_font.reset();
    }
    void Console::update() {
        WG_AUTO_PROFILE_DEBUG("Console::update");

        std::lock_guard lock(m_mutex);

        auto dt = float(Engine::instance()->get_delta_time());

        m_state_open = m_state_open + m_current_speed * dt;
        if (m_state_open > 1.0f) {
            m_state_open    = 1.0f;
            m_current_speed = 0.0f;
            m_state         = ConsoleState::Open;
        }
        if (m_state_open < 0.0f) {
            m_state_open    = 0.0f;
            m_current_speed = 0.0f;
            m_state         = ConsoleState::Closed;
        }

        m_state_blink = m_state_blink + m_speed_blink * dt;
        if (m_state_blink > 1.0f) {
            m_state_blink = 0.0;
        }
    }
    void Console::render() {
        WG_AUTO_PROFILE_DEBUG("Console::render");

        std::lock_guard lock(m_mutex);

        if (m_state == ConsoleState::Closed) return;

        auto  screen = m_canvas->get_screen_size();
        float height = m_size * m_state_open * screen.y();
        float width  = screen.x();

        m_canvas->push(Vec2f(0.0f, screen.y() - height), 0.0f);
        m_canvas->set_font(m_console_font);

        m_canvas->set_fill_color(m_color_back);
        m_canvas->draw_filled_rect(Vec2f(0, 0), Vec2f(width, height));

        m_canvas->set_fill_color(m_color_line);
        m_canvas->draw_filled_rect(Vec2f(0, 0), Vec2f(width, m_line_size));

        m_canvas->set_font_color(m_color_text);
        m_canvas->draw_text(">", Vec2f(m_margin, m_text_line), m_text_size);

        if (!m_line.empty()) {
            m_canvas->set_font_color(m_color_text);
            m_canvas->draw_text(m_line, Vec2f(m_margin_line, m_text_line), m_text_size);
        }

        if (m_state_blink > m_blink_threshold) {
            m_canvas->set_fill_color(m_color_cursor);
            m_canvas->draw_filled_rect(Vec2f(m_margin_line + m_cursor_offset, m_margin), Vec2f(m_cursor_width, m_cursor_height));
        }

        float text_pos = m_line_size + m_margin;
        for (int i = static_cast<int>(m_messages.size()) - m_scroll_messages - 1; i >= 0; --i) {
            if (text_pos > height) break;
            m_canvas->set_font_color(m_messages[i].color);
            m_canvas->draw_text(m_messages[i].text, Vec2f(m_margin, text_pos), m_text_size);
            text_pos += m_text_size;
        }

        m_max_to_display = static_cast<int>((height - m_line_size - m_margin) / m_text_size);
        m_canvas->pop();
    }

    void Console::register_commands() {
        register_cmd(SID("clear"), "Clear all console messages", [=](const auto&) {
            m_messages.clear();
            return 0;
        });

        register_cmd(SID("help"), "Show help info about console object", [=](const auto& args) {
            if (args.size() < 2) {
                add_error("not enough args");
                return 0;
            }

            StringId name(args[1]);

            if (auto var = find_var(name)) {
                add_suggestion(var->get_help());
                return 0;
            }
            if (auto cmd = find_cmd(name)) {
                add_suggestion(cmd->get_help());
                return 0;
            }

            add_error("no such object " + name.str());
            return 0;
        });

        register_cmd(SID("get"), "Get current value of the variable", [=](const auto& args) {
            if (args.size() < 2) {
                add_error("not enough args");
                return 0;
            }

            StringId name(args[1]);

            if (auto var = find_var(name)) {
                add_info(var->get_value().to_string());
                return 0;
            }

            add_error("no such var " + name.str());
            return 0;
        });

        register_cmd(SID("list"), "List console objects matching provided prefix", [=](const auto& args) {
            std::string m_prefix;
            if (args.size() > 1) {
                m_prefix = args[1];
            }

            std::lock_guard                   lock(m_mutex);
            std::vector<const ConsoleObject*> matching;
            std::size_t                       largest_name = 0;

            for (const auto& entry : m_cmds) {
                if (entry.first.str().find(m_prefix) == 0) {
                    matching.push_back(&entry.second);
                    largest_name = Math::max(largest_name, entry.second.m_name.str().length());
                }
            }
            for (const auto& entry : m_vars) {
                if (entry.first.str().find(m_prefix) == 0) {
                    matching.push_back(&entry.second);
                    largest_name = Math::max(largest_name, entry.second.m_name.str().length());
                }
            }

            std::sort(matching.begin(), matching.end(), [](const auto& a, const auto& b) { return a->get_name() < b->get_name(); });

            for (auto object : matching) {
                std::stringstream line;
                std::size_t       help_to_show = 80;
                std::size_t       name_length  = object->get_name().str().length();

                line << " * " << object->get_name().str();
                for (std::size_t i = 0; i < largest_name - name_length; i++) line << " ";
                line << " (" << (object->is_var() ? "var" : "cmd") << ") " << object->get_help().substr(0, help_to_show) << "...";

                add_message_internal(line.str(), m_color_suggestion, true);
            }

            return 0;
        });

        register_cmd(SID("set"), "Set new value to the variable", [=](const auto& args) {
            if (args.size() < 3) {
                add_error("not enough args");
                return 0;
            }

            StringId name(args[1]);
            Var      value(args[2]);

            if (auto var = find_var(name)) {
                var->change(std::move(value));
                return 0;
            }

            add_error("no such var " + name.str());
            return 0;
        });
    }
    void Console::load_settings() {
        WG_AUTO_PROFILE_DEBUG("Console::load_settings");

        auto engine  = Engine::instance();
        auto res_man = engine->resource_manager();
        auto config  = engine->config();

        m_canvas       = engine->canvas_2d_debug();
        m_console_font = res_man->load(SID(config->get_string(SID("debug.console.font"), "res://fonts/anonymous_pro"))).cast<Font>();
        m_margin_line  = m_margin + m_console_font->get_string_size("> ", m_text_size).x();

        config->get(SID("debug.console.color_back"), m_color_back);
        config->get(SID("debug.console.color_line"), m_color_line);
        config->get(SID("debug.console.color_text"), m_color_text);
        config->get(SID("debug.console.color_input"), m_color_input);
        config->get(SID("debug.console.color_warning"), m_color_warning);
        config->get(SID("debug.console.color_error"), m_color_error);
        config->get(SID("debug.console.color_suggestion"), m_color_suggestion);
        config->get(SID("debug.console.color_cursor"), m_color_cursor);
        config->get(SID("debug.console.speed_open"), m_speed_open);
        config->get(SID("debug.console.speed_blink"), m_speed_blink);
        config->get(SID("debug.console.blink_threshold"), m_blink_threshold);
        config->get(SID("debug.console.size"), m_size);
        config->get(SID("debug.console.text_size"), m_text_size);
        config->get(SID("debug.console.line_size"), m_line_size);
        config->get(SID("debug.console.cursor_width"), m_cursor_width);
        config->get(SID("debug.console.cursor_height"), m_cursor_height);
        config->get(SID("debug.console.text_line"), m_text_line);
        config->get(SID("debug.console.margin"), m_margin);
        config->get(SID("debug.console.margin_line"), m_margin_line);
    }
    void Console::add_message_internal(const std::string& message, const Color4f& color, bool merge_lines) {
        std::lock_guard lock(m_mutex);

        std::vector<std::string> lines;
        StringUtils::split(message, "\n", lines);

        if (merge_lines) {
            std::stringstream merged;
            for (auto& line : lines) {
                merged << line << " ";
            }
            auto& entry = m_messages.emplace_back();
            entry.text  = std::move(merged.str());
            entry.color = color;
            return;
        }

        for (auto& line : lines) {
            auto& entry = m_messages.emplace_back();
            entry.text  = std::move(line);
            entry.color = color;
        }
    }

}// namespace wmoge

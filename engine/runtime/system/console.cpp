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

#include "asset/asset_manager.hpp"
#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "math/math_utils.hpp"
#include "platform/time.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler.hpp"
#include "render/canvas.hpp"

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

    ConsoleVar* Console::register_var(Strid name, Var default_value, std::string help, std::function<void(const Var&, const Var&)> on_changed) {
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
    ConsoleCmd* Console::register_cmd(Strid name, std::string help, std::function<int(const std::vector<std::string>&)> function) {
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
    ConsoleVar* Console::find_var(const Strid& name) {
        std::lock_guard lock(m_mutex);
        auto            query = m_vars.find(name);
        return query != m_vars.end() ? &query->second : nullptr;
    }
    ConsoleCmd* Console::find_cmd(const Strid& name) {
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

    void Console::init(AssetManager* asset_manager) {
        register_commands();
        load_settings(asset_manager);
    }

    void Console::shutdown() {
        m_console_font.reset();
    }

    void Console::update() {
        WG_AUTO_PROFILE_DEBUG("Console::update");

        std::lock_guard lock(m_mutex);

        auto dt = 0.0f;// float(Engine::instance()->time()->get_delta_time());

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

        auto  screen = Vec2f(1280.0f, 720.0f);
        float height = m_size * m_state_open * screen.y();
        float width  = screen.x();

        m_canvas->push_transform(Vec2f(0.0f, screen.y() - height), 0.0f);

        m_canvas->add_rect_filled(Vec2f(0, 0), Vec2f(width, height), m_color_back);
        m_canvas->add_rect_filled(Vec2f(0, 0), Vec2f(width, m_line_size), m_color_line);
        m_canvas->add_text(">", m_console_font, m_text_size, Vec2f(m_margin, m_text_line), m_color_text);

        if (!m_line.empty()) {
            m_canvas->add_text(m_line, m_console_font, m_text_size, Vec2f(m_margin_line, m_text_line), m_color_text);
        }

        if (m_state_blink > m_blink_threshold) {
            const auto p_min = Vec2f(m_margin_line + m_cursor_offset, m_margin);
            const auto p_max = p_min + Vec2f(m_cursor_width, m_cursor_height);
            m_canvas->add_rect_filled(p_min, p_max, m_color_cursor);
        }

        float text_pos = m_line_size + m_margin;
        for (int i = static_cast<int>(m_messages.size()) - m_scroll_messages - 1; i >= 0; --i) {
            if (text_pos > height) break;
            m_canvas->add_text(m_messages[i].text, m_console_font, m_text_size, Vec2f(m_margin, text_pos), m_messages[i].color);
            text_pos += m_text_size;
        }

        m_max_to_display = static_cast<int>((height - m_line_size - m_margin) / m_text_size);
        m_canvas->pop_transform();
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

            Strid name(args[1]);

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

            Strid name(args[1]);

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

            Strid name(args[1]);
            Var   value(args[2]);

            if (auto var = find_var(name)) {
                var->change(std::move(value));
                return 0;
            }

            add_error("no such var " + name.str());
            return 0;
        });
    }
    void Console::load_settings(AssetManager* asset_manager) {
        WG_AUTO_PROFILE_DEBUG("Console::load_settings");

        const std::string font_name = "assets/fonts/anonymous_pro";

        // m_canvas       = Engine::instance()->canvas_debug();
        m_console_font = asset_manager->load(AssetId(font_name)).cast<Font>();
        m_margin_line  = m_margin + m_console_font->get_string_size("> ", m_text_size).x();
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

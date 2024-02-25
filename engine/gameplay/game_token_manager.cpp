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

#include "game_token_manager.hpp"

#include "core/log.hpp"
#include "event/event_manager.hpp"
#include "event/event_token.hpp"
#include "system/engine.hpp"

namespace wmoge {

    GameTokenManager::GameTokenManager() {
        m_event_manager = Engine::instance()->event_manager();
    }

    void GameTokenManager::set(const StringId& token, int value) {
        set(token, Var(value));
    }
    void GameTokenManager::set(const StringId& token, float value) {
        set(token, Var(value));
    }
    void GameTokenManager::set(const StringId& token, std::string value) {
        set(token, Var(std::move(value)));
    }
    void GameTokenManager::set(const StringId& token, Var value) {
        bool has_already = m_tokens.find(token) != m_tokens.end();
        m_tokens[token]  = std::move(value);

        auto event          = make_event<EventToken>();
        event->token        = token;
        event->notification = has_already ? TokenNotification::Updated : TokenNotification::Added;
        m_event_manager->dispatch(event);

        if (!has_already) {
            WG_LOG_INFO("set new token " << token);
        }
    }

    bool GameTokenManager::get(const StringId& token, int& value) {
        auto query = m_tokens.find(token);
        if (query != m_tokens.end()) {
            value = query->second.operator int();
            return true;
        }
        WG_LOG_ERROR("no such token " << token);
        return false;
    }
    bool GameTokenManager::get(const StringId& token, float& value) {
        auto query = m_tokens.find(token);
        if (query != m_tokens.end()) {
            value = query->second.operator float();
            return true;
        }
        WG_LOG_ERROR("no such token " << token);
        return false;
    }
    bool GameTokenManager::get(const StringId& token, std::string& value) {
        auto query = m_tokens.find(token);
        if (query != m_tokens.end()) {
            value = query->second.operator std::string();
            return true;
        }
        WG_LOG_ERROR("no such token " << token);
        return false;
    }
    bool GameTokenManager::get(const StringId& token, Var& value) {
        auto query = m_tokens.find(token);
        if (query != m_tokens.end()) {
            value = query->second;
            return true;
        }
        WG_LOG_ERROR("no such token " << token);
        return false;
    }

}// namespace wmoge

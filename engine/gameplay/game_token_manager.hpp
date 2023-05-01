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

#ifndef WMOGE_GAME_TOKEN_MANAGER_HPP
#define WMOGE_GAME_TOKEN_MANAGER_HPP

#include "core/fast_map.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"

namespace wmoge {

    /**
     * @class GameTokenManager
     * @brief Global storage for game tokens shared across engine
     *
     * Allows to store game, scene and level state in a form of tokens.
     * Token is a named and typed variable. Tokens can be saved, loaded,
     * modified. It is a useful for programming of a global a game score, state, etc.
     */
    class GameTokenManager final {
    public:
        GameTokenManager();

        void set(const StringId& token, int value);
        void set(const StringId& token, float value);
        void set(const StringId& token, std::string value);
        void set(const StringId& token, Var value);

        bool get(const StringId& token, int& value);
        bool get(const StringId& token, float& value);
        bool get(const StringId& token, std::string& value);
        bool get(const StringId& token, Var& value);

    private:
        fast_map<StringId, Var> m_tokens;
        class EventManager*     m_event_manager;
    };

}// namespace wmoge

#endif//WMOGE_GAME_TOKEN_MANAGER_HPP

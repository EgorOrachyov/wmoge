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
#include "core/simple_id.hpp"
#include "core/status.hpp"

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace wmoge {

    /** @brief Signal bind id */
    using SignalBindId = SimpleId<>;

    /**
     * @class Signal
     * @brief Allows to immediately notify listeners when something occurse on singnal holder side
    */
    template<typename... TArgs>
    class Signal {
    public:
        using Callback = std::function<void(TArgs... args)>;

        Signal()                  = default;
        Signal(const Signal&)     = delete;
        Signal(Signal&&) noexcept = delete;

        void         emit(TArgs&&... args);
        SignalBindId bind(Callback&& callback);
        bool         unbind(SignalBindId id);
        void         clear();

    private:
        struct Client {
            Callback     callback;
            SignalBindId id;
        };

        buffered_vector<Client, 2> m_callbacks;
        SignalBindId               m_next_id{0};
    };

    template<typename... TArgs>
    inline void Signal<TArgs...>::emit(TArgs&&... args) {
        for (const auto& client : m_callbacks) {
            client.callback(std::forward<TArgs>(args)...);
        }
    }

    template<typename... TArgs>
    inline SignalBindId Signal<TArgs...>::bind(Callback&& callback) {
        Client& client  = m_callbacks.emplace_back();
        client.callback = std::move(callback);
        client.id       = m_next_id;
        ++m_next_id;
        return client.id;
    }

    template<typename... TArgs>
    inline bool Signal<TArgs...>::unbind(SignalBindId id) {
        auto target = std::find_if(m_callbacks.begin(), m_callbacks.end(), [&](const Client& c) {
            return c.id == id;
        });
        if (target != m_callbacks.end()) {
            m_callbacks.erase(target);
            return true;
        }
        return false;
    }

    template<typename... TArgs>
    inline void Signal<TArgs...>::clear() {
        m_callbacks.clear();
    }

}// namespace wmoge
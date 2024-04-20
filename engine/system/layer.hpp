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

#include "core/array_view.hpp"

#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class Layer
     * @brief Interface for a general engine layer for execution
     */
    class Layer {
    public:
        virtual ~Layer() = default;

        virtual void on_attach() {}
        virtual void on_start_frame() {}
        virtual void on_iter() {}
        virtual void on_debug_draw() {}
        virtual void on_end_frame() {}
    };

    /**
     * @class LayerStack
     * @brief Stack of attached engine layers
     */
    class LayerStack {
    public:
        LayerStack()                  = default;
        LayerStack(const LayerStack&) = delete;
        LayerStack(LayerStack&&)      = delete;
        ~LayerStack()                 = default;

        using LayerPtr     = std::shared_ptr<Layer>;
        using LayerStorage = std::vector<LayerPtr>;

        template<typename F>
        void each_up(F&& f);
        template<typename F>
        void each_down(F&& f);

        void attach(LayerPtr layer);
        void remove(LayerPtr layer);
        void clear();

        [[nodiscard]] array_view<const LayerPtr> get_layers() const { return m_layers; }
        [[nodiscard]] std::size_t                get_size() const { return m_layers.size(); }
        [[nodiscard]] bool                       is_empty() const { return m_layers.empty(); }

    private:
        LayerStorage m_layers;
    };

    template<typename F>
    inline void LayerStack::each_up(F&& f) {
        for (auto it = m_layers.begin(); it != m_layers.end(); ++it) {
            f(*it);
        }
    }

    template<typename F>
    inline void LayerStack::each_down(F&& f) {
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) {
            f(*it);
        }
    }

}// namespace wmoge
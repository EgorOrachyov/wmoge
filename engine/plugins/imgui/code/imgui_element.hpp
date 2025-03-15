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

#include <functional>

namespace wmoge {

    class ImguiManager;

    /**
     * @class ImguiProcessContext
     * @brief Context for imgui 'draw' ui elements pass
     */
    class ImguiProcessContext {
    public:
        ImguiProcessContext() = default;

        void add_action(std::function<void()> action);
        void exec_actions();

    private:
        std::vector<std::function<void()>> m_actions;
    };

    /**
     * @class ImguiElement
     * @brief Base class for all imgui backend ui elements
     */
    class ImguiElement {
    public:
        ImguiElement(ImguiManager* manager);
        virtual ~ImguiElement() = default;

        virtual void process(ImguiProcessContext& context) {}

    protected:
        ImguiManager* m_manager;
    };

    /**
     * @class ImguiElementBase
     * @brief Helper class to implement ui element
     */
    template<typename UiBaseClass>
    class ImguiElementBase : public UiBaseClass, public ImguiElement {
    public:
        ImguiElementBase(ImguiManager* manager) : ImguiElement(manager) {}
        ~ImguiElementBase() override = default;
    };

}// namespace wmoge
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

#include "gfx/gfx_driver.hpp"
#include "imgui_driver.hpp"
#include "imgui_element.hpp"
#include "imgui_factory.hpp"
#include "imgui_platform.hpp"
#include "platform/window_manager.hpp"
#include "ui/ui_manager.hpp"

#include <memory>

namespace wmoge {

    /**
     * @class ImguiManager
     * @brief Implementation of ui manager fom imgui backend
     */
    class ImguiManager : public UiManager {
    public:
        ImguiManager(WindowManager* window_manager, GfxDriver* driver);
        ~ImguiManager() override;

        void       provide_window(Ref<UiMainWindow> window) override;
        void       update() override;
        void       render(const GfxCmdListRef& cmd_list) override;
        UiFactory* get_factory() override;

        [[nodiscard]] bool is_docking_enable() const { return m_docking_enable; }
        [[nodiscard]] bool is_viewports_enable() const { return m_viewports_enable; }

    protected:
        void process_main_window(ImguiProcessContext& context);
        void dispatch_actions(ImguiProcessContext& context);

    private:
        std::unique_ptr<ImguiPlatform> m_platform;
        std::unique_ptr<ImguiDriver>   m_driver;
        std::unique_ptr<ImguiFactory>  m_factory;
        Ref<UiMainWindow>              m_main_window;

        bool m_docking_enable   = true;
        bool m_viewports_enable = true;

        bool m_show_demo_window = true;
    };

}// namespace wmoge
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
#include "gfx/gfx_texture.hpp"
#include "grc/texture.hpp"
#include "imgui_driver.hpp"
#include "imgui_platform.hpp"
#include "imgui_process.hpp"
#include "imgui_style.hpp"
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

        void         set_main_window(Ref<UiMainWindow> window) override;
        void         add_dock_window(Ref<UiDockWindow> window) override;
        void         update(std::size_t frame_id) override;
        void         render(const GfxCmdListRef& cmd_list) override;
        void         set_style(const Ref<UiStyle>& style) override;
        Ref<UiStyle> get_style() override;
        Ref<UiStyle> get_style_default() override;

        ImTextureID get_texture_id(const Ref<Texture2d>& texture);

        [[nodiscard]] bool is_docking_enable() const { return m_docking_enable; }
        [[nodiscard]] bool is_viewports_enable() const { return m_viewports_enable; }

    protected:
        void    process_main_window();
        void    process_dock_windows();
        void    dispatch_actions();
        ImFont* load_font(const Ref<Font>& font);

    private:
        std::unique_ptr<ImguiProcessor> m_processor;
        std::unique_ptr<ImguiPlatform>  m_platform;
        std::unique_ptr<ImguiDriver>    m_driver;
        std::vector<Ref<UiDockWindow>>  m_dock_windows;
        Ref<UiMainWindow>               m_main_window;
        Ref<UiStyle>                    m_style;
        Ref<UiStyle>                    m_style_default;
        std::vector<ImFont*>            m_fonts;
        flat_map<Ref<Font>, ImFont*>    m_loaded_fonts;

        bool m_docking_enable   = true;
        bool m_viewports_enable = true;
        bool m_show_demo_window = true;
    };

}// namespace wmoge
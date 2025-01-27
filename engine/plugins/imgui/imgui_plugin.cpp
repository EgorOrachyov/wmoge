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

#include "imgui_plugin.hpp"

#include "_rtti.hpp"
#include "core/log.hpp"
#include "gfx/gfx_driver.hpp"
#include "imgui_manager.hpp"
#include "platform/window_manager.hpp"

namespace wmoge {

    ImguiPlugin::ImguiPlugin() {
        m_name         = SID("imgui");
        m_uuid         = UUID::generate();
        m_description  = "Standard engine plugin with ui backend based on imgui";
        m_requirements = {};
    }

    Status ImguiPlugin::on_register(IocContainer* ioc) {
        rtti_imgui();

        m_ioc = ioc;

        ioc->bind_by_factory<ImguiManager>([ioc]() {
            WindowManager* window_manager = ioc->resolve_value<WindowManager>();
            GfxDriver*     driver         = ioc->resolve_value<GfxDriver>();
            return std::make_shared<ImguiManager>(window_manager, driver);
        });

        ioc->bind_by_factory<UiManager>([ioc]() {
            return std::shared_ptr<UiManager>(ioc->resolve_value<ImguiManager>(), [](auto p) {});
        });

        WG_LOG_INFO("init imgui plugin");

        return WG_OK;
    }

    Status ImguiPlugin::on_shutdown() {
        m_ioc->unbind<ImguiManager>();

        return WG_OK;
    }

}// namespace wmoge
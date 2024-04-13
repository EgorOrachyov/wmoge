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

#include "image_plugin.hpp"

#include "asset/asset_manager.hpp"
#include "core/log.hpp"
#include "core/status.hpp"
#include "image_asset_loader.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    ImagePlugin::ImagePlugin() {
        m_name         = SID("iamge");
        m_uuid         = UUID::generate();
        m_description  = "Brings image utilities and import support into the engine";
        m_requirements = {};
    }

    Status ImagePlugin::on_register() {
        IocContainer* ioc_container = IocContainer::instance();
        AssetManager* asset_manager = ioc_container->resolve_v<AssetManager>();

        asset_manager->add_loader(std::make_shared<ImageAssetLoader>());

        WG_LOG_INFO("init image plugin");

        return StatusCode::Ok;
    }

}// namespace wmoge
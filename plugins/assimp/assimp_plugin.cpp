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

#include "assimp_plugin.hpp"

#include "assimp_resource_loader.hpp"
#include "core/log.hpp"
#include "core/status.hpp"
#include "resource/resource_manager.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    AssimpPlugin::AssimpPlugin() {
        m_name         = SID("assimp");
        m_uuid         = UUID::generate();
        m_description  = "Brings assimp import library support for mesh and animation data";
        m_requirements = {};
    }

    Status AssimpPlugin::on_register() {
        IocContainer*    ioc_container    = IocContainer::instance();
        ResourceManager* resource_manager = ioc_container->resolve_v<ResourceManager>();

        resource_manager->add_loader(std::make_shared<AssimpResourceLoader>());

        WG_LOG_INFO("init assimp plugin");

        return StatusCode::Ok;
    }

}// namespace wmoge
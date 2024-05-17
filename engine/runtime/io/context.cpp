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

#include "context.hpp"

#include "asset/asset_manager.hpp"
#include "rtti/type_storage.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    AssetManager* wmoge::IoContext::get_asset_manager() {
        if (!m_asset_manager) {
            m_asset_manager = get_ioc_container()->resolve_v<AssetManager>();
        }
        return m_asset_manager;
    }

    RttiTypeStorage* IoContext::get_type_storage() {
        if (!m_type_storage) {
            m_type_storage = get_ioc_container()->resolve_v<RttiTypeStorage>();
        }
        return m_type_storage;
    }

    IocContainer* IoContext::get_ioc_container() {
        if (!m_ioc_container) {
            m_ioc_container = IocContainer::instance();
        }
        return m_ioc_container;
    }

}// namespace wmoge
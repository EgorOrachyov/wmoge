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

#include "_bind.hpp"

#include "core/ioc_container.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "io/config_manager.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    void bind_grc(IocContainer* ioc) {
        ioc->bind_by_ioc<ShaderLibrary>();
        ioc->bind_by_ioc<PsoCache>();
        ioc->bind_by_ioc<TextureManager>();

        ioc->bind_by_factory<ShaderTaskManager>([ioc]() {
            const int num_workers = 4;
            return std::make_shared<ShaderTaskManager>(num_workers);
        });

        ioc->bind_by_factory<ShaderManager>([ioc]() {
            return std::make_shared<ShaderManager>(
                    ioc->resolve_value<ShaderTaskManager>(),
                    ioc->resolve_value<FileSystem>(),
                    ioc->resolve_value<GfxDriver>(),
                    ioc->resolve_value<TextureManager>(),
                    ioc->resolve_value<ShaderLibrary>(),
                    ioc->resolve_value<PsoCache>(),
                    ioc->resolve_value<CfgManager>());
        });
    }

    void unbind_grc(IocContainer* ioc) {
        ioc->unbind<ShaderManager>();
        ioc->unbind<ShaderLibrary>();
        ioc->unbind<PsoCache>();
        ioc->unbind<ShaderTaskManager>();
        ioc->unbind<TextureManager>();
    }

}// namespace wmoge
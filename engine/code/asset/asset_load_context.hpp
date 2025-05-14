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
#include "core/data.hpp"
#include "io/context.hpp"
#include "rtti/object.hpp"
#include "rtti/type_ref.hpp"

#include <string>

namespace wmoge {
    class IocContainer;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetLoadContext
     * @brief Context passed to the asset loader
     */
    class AssetLoadContext {
    public:
        AssetLoadContext(std::string                 name,
                         IoContext                   io_context,
                         IocContainer*               ioc_container,
                         RttiRefClass                cls,
                         array_view<Ref<RttiObject>> artifacts,
                         array_view<Ref<Data>>       buffers);

        [[nodiscard]] const std::string&                get_name() const { return m_name; }
        [[nodiscard]] IoContext&                        get_io_context() { return m_io_context; }
        [[nodiscard]] IocContainer*                     get_ioc_container() const { return m_ioc_container; }
        [[nodiscard]] RttiRefClass                      get_cls() const { return m_cls; }
        [[nodiscard]] array_view<const Ref<RttiObject>> get_artifacts() const { return m_artifacts; }
        [[nodiscard]] array_view<const Ref<Data>>       get_buffers() const { return m_buffers; }

    private:
        std::string                 m_name;
        IoContext                   m_io_context;
        IocContainer*               m_ioc_container;
        RttiRefClass                m_cls;
        array_view<Ref<RttiObject>> m_artifacts;
        array_view<Ref<Data>>       m_buffers;
    };

}// namespace wmoge
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

#include "core/flat_map.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "core/synchronization.hpp"
#include "rtti/class.hpp"
#include "rtti/type.hpp"

#include <cinttypes>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>

namespace wmoge {

    /**
     * @class RttiTypeStorage
     * @brief Engine global storage for rtti registered types
    */
    class RttiTypeStorage {
    public:
        RttiTypeStorage()  = default;
        ~RttiTypeStorage() = default;

        std::optional<RttiType*> find_type(const Strid& name);
        RttiStruct*              find_struct(const Strid& name);
        RttiClass*               find_class(const Strid& name);
        bool                     has(const Strid& name);
        void                     add(const Ref<RttiType>& type);
        std::vector<RttiType*>   find_types(const std::function<bool(const Ref<RttiType>&)>& filter);
        std::vector<RttiClass*>  find_classes(const std::function<bool(const Ref<RttiClass>&)>& filter);

        static RttiTypeStorage* instance();
        static void             provide(RttiTypeStorage* storage);

    private:
        flat_map<Strid, Ref<RttiType>> m_types;
        mutable std::recursive_mutex   m_mutex;

        static RttiTypeStorage* g_storage;
    };

}// namespace wmoge
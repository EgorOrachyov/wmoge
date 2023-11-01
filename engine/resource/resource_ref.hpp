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

#ifndef WMOGE_RESOURCE_REF_HPP
#define WMOGE_RESOURCE_REF_HPP

#include "core/engine.hpp"
#include "io/yaml.hpp"
#include "resource/resource.hpp"
#include "resource/resource_manager.hpp"

#include <cassert>
#include <optional>
#include <type_traits>

namespace wmoge {

    /**
     * @class ResRef
     * @brief Aux box to store resource ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class ResRef : public Ref<T> {
    public:
        static_assert(std::is_base_of_v<Resource, T>, "Must be a resource");

        ResRef() = default;
        ResRef(Ref<T> ptr) : Ref<T>(std::move(ptr)) {}
    };

    /**
     * @class ResRefWeak
     * @brief Aux box to store resource ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class ResRefWeak : public ResourceId {
    public:
        static_assert(std::is_base_of_v<Resource, T>, "Must be a resource");

        ResRefWeak() = default;
        ResRefWeak(const ResourceId& id) : ResourceId(id) {}
        ResRefWeak(const ResRef<T>& ref) : ResourceId(ref ? ref->get_id() : ResourceId()) {}
    };

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, ResRef<T>& ref) {
        ResourceId id;
        WG_YAML_READ(node, id);
        Ref<T> ptr = Engine::instance()->resource_manager()->load(id).cast<T>();
        if (!ptr) {
            return StatusCode::NoResource;
        }
        ref = ResRef<T>(ptr);
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const ResRef<T>& ref) {
        assert(ref);
        if (!ref) {
            return StatusCode::NoResource;
        }
        WG_YAML_WRITE(node, ref->get_id());
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, ResRefWeak<T>& ref) {
        ResourceId id;
        WG_YAML_READ(node, id);
        ref = ResRefWeak<T>(id);
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const ResRefWeak<T>& ref) {
        ResourceId id = ref;
        WG_YAML_WRITE(node, id);
        return StatusCode::Ok;
    }

}// namespace wmoge

#endif//WMOGE_RESOURCE_REF_HPP
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

namespace wmoge {

    /**
     * @class ResourceRefWeak
     * @brief Aux box to store resource ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class ResourceRefWeak {
    public:
        ResourceRefWeak() = default;

    private:
        ResourceId m_id;
    };

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, ResourceRefWeak<T>& ref) {
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const ResourceRefWeak<T>& ref) {
        return StatusCode::Ok;
    }

    /**
     * @class ResourceRefHard
     * @brief Aux box to store resource ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class ResourceRefHard {
    public:
        ResourceRefHard() = default;
        ResourceRefHard(Ref<T> ptr) : m_ptr(std::move(ptr)) {}

        [[nodiscard]] std::optional<Ref<T>> get() const {
            return m_ptr ? std::optional<Ref<T>>(m_ptr) : std::optional<Ref<T>>();
        }

        [[nodiscard]] Ref<T> get_unsafe() const {
            return m_ptr;
        }

        [[nodiscard]] Ref<T> get_safe() const {
            assert(m_ptr);
            return m_ptr;
        }

    private:
        Ref<T> m_ptr;
    };

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, ResourceRefHard<T>& ref) {
        ResourceId id;
        WG_YAML_READ(node, id);
        Ref<T> ptr = Engine::instance()->resource_manager()->load(id).cast<T>();
        if (!ptr) {
            return StatusCode::NoResource;
        }
        ref = ResourceRefHard<T>(ptr);
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const ResourceRefHard<T>& ref) {
        assert(ref.get().has_value());
        if (!ref.get().has_value()) {
            return StatusCode::NoResource;
        }
        WG_YAML_WRITE(node, ref.get_safe()->get_name());
        return StatusCode::Ok;
    }

}// namespace wmoge

#endif//WMOGE_RESOURCE_REF_HPP
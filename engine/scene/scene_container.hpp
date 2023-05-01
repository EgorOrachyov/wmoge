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

#ifndef WMOGE_SCENE_CONTAINER_HPP
#define WMOGE_SCENE_CONTAINER_HPP

#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "math/math_utils.hpp"
#include "scene/scene_component.hpp"

#include <cassert>
#include <memory>
#include <mutex>
#include <optional>

namespace wmoge {

    /**
     * @class SceneContainerPtr
     * @brief Interface to container with weak refs of objects and components
     */
    class SceneContainerPtr {
    public:
        virtual ~SceneContainerPtr()                        = default;
        virtual void               add(Object* object)      = 0;
        virtual void               remove(Object* object)   = 0;
        virtual bool               contains(Object* object) = 0;
        virtual const class Class* get_class()              = 0;
    };

    /**
     * @class TSceneContainerPtr
     * @brief Container to store weak references for objects and components
     *
     * @tparam T Type of referenced objects
     */
    template<typename T>
    class TSceneContainerPtr final : public SceneContainerPtr {
    public:
        TSceneContainerPtr() {
            m_class = T::class_ptr_static();
        }

        void add(Object* object) override {
            m_objects.insert(reinterpret_cast<T*>(object));
        }

        void remove(Object* object) override {
            m_objects.erase(reinterpret_cast<T*>(object));
        }

        bool contains(Object* object) override {
            return m_objects.find(reinterpret_cast<T*>(object)) != m_objects.end();
        }

        template<typename Functor>
        void for_each(Functor&& f) {
            for (auto object : m_objects) {
                f(*object);
            }
        }

        template<typename Predicate>
        std::optional<T*> first_matching(Predicate&& p) {
            for (auto object : m_objects) {
                if (p(*object)) {
                    return object;
                }
            }
            return std::nullopt;
        }

        const Class* get_class() override {
            return m_class;
        }

    private:
        fast_set<T*> m_objects;
        const Class* m_class = nullptr;
    };

    /**
     * @class SceneContainerMem
     * @brief Container to allocate objects and components
     */
    class SceneContainerMem {
    public:
        virtual ~SceneContainerMem()                  = default;
        virtual Object*            create()           = 0;
        virtual void               destroy(Object* c) = 0;
        virtual const class Class* get_class()        = 0;
    };

    /**
     * @class TSceneContainerMem
     * @brief Container to allocate objects and components
     *
     * @tparam T Type of referenced objects
     */
    template<typename T>
    class TSceneContainerMem final : public SceneContainerMem {
    public:
        TSceneContainerMem() {
            m_class = T::class_ptr_static();
        }

        ~TSceneContainerMem() {
            assert(m_allocated.empty());

            WG_LOG_INFO("free total " << m_free.size() << " of " << m_class->name() << " ("
                                      << StringUtils::from_mem_size(m_free.size() * m_cmp_size) << ")");

            for (auto c : m_free) {
                std::free(c);
            }
        }

        Object* create() override {
            Object* mem;
            {
                std::lock_guard guard(m_mutex);

                if (m_free.empty()) {
                    m_free.push_back(std::malloc(m_cmp_size));
                }

                mem = reinterpret_cast<Object*>(m_free.back());
                m_allocated.insert(mem);
                m_free.pop_back();
            }
            return new (mem) T();
        }

        void destroy(Object* c) override {
            assert(c->class_ptr() == m_class);
            c->~Object();
            {
                std::lock_guard guard(m_mutex);
                m_free.push_back(c);
                m_allocated.erase(c);
            }
        }

        const Class* get_class() override {
            return m_class;
        }

    private:
        static const std::size_t ALIGNMENT = 64;
        fast_set<Object*>        m_allocated;
        fast_vector<void*>       m_free;
        std::mutex               m_mutex;
        std::size_t              m_cmp_size = Math::align(sizeof(T), ALIGNMENT);
        const Class*             m_class    = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_CONTAINER_HPP

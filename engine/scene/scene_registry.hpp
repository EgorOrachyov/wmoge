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

#ifndef WMOGE_SCENE_REGISTRY_HPP
#define WMOGE_SCENE_REGISTRY_HPP

#include "core/class.hpp"
#include "core/fast_map.hpp"
#include "scene/scene_container.hpp"

namespace wmoge {

    /**
     * @class SceneRegistry
     * @brief Registry of scene objects and scene components
     */
    class SceneRegistry {
    public:
        template<typename T>
        void register_type() {
            auto* cls   = T::class_ptr_static();
            auto  query = m_registry.find(cls);
            if (query == m_registry.end()) {
                m_registry[cls] = std::make_unique<TSceneContainerPtr<T>>();
            }
        }

        SceneContainerPtr* get_container(const Class* cls) {
            auto query = m_registry.find(cls);
            if (query != m_registry.end()) {
                return query->second.get();
            }
            return nullptr;
        }

        template<typename T>
        TSceneContainerPtr<T>* get_container() {
            auto* cls = T::class_ptr_static();
            return reinterpret_cast<TSceneContainerPtr<T>*>(get_container(cls));
        }

        template<typename T>
        void add(T* ptr) {
            get_container<T>()->add(ptr);
        }

        template<typename T>
        void remove(T* ptr) {
            get_container<T>()->remove(ptr);
        }

        template<typename T, typename Functor>
        void for_each(Functor&& f) {
            TSceneContainerPtr<T>* container = get_container<T>();
            assert(container);
            container->for_each(std::forward<Functor>(f));
        }

        template<typename T, typename Predicate>
        std::optional<T*> first_matching(Predicate&& p) {
            TSceneContainerPtr<T>* container = get_container<T>();
            assert(container);
            return container->first_matching(std::forward<Predicate>(p));
        }

    private:
        fast_map<const Class*, std::unique_ptr<SceneContainerPtr>> m_registry;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_REGISTRY_HPP

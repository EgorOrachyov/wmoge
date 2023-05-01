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

#ifndef WMOGE_SCENE_COMPONENT_HPP
#define WMOGE_SCENE_COMPONENT_HPP

#include "core/class.hpp"
#include "core/object.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "io/yaml.hpp"

#include <array>

namespace wmoge {

    /**
     * @class SceneComponent
     * @brief Is a base class for any component which can be attached to the scene object
     */
    class SceneComponent : public Object {
    public:
        WG_OBJECT(SceneComponent, Object)

        /** @return Scene this component belongs to */
        class Scene* get_scene();
        /** @return Scene object this component belongs to */
        class SceneObject* get_scene_object();
        /** @return True if object in a scene */
        bool is_in_scene() const;

        virtual bool on_load_from_yaml(const YamlConstNodeRef& node) { return true; }
        virtual void on_create() {}
        virtual void on_scene_enter();
        virtual void on_scene_exit();
        virtual void on_transform_updated() {}

    protected:
        void destroy() override;

    private:
        friend class SceneObject;
        class SceneObject* m_scene_object;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_COMPONENT_HPP

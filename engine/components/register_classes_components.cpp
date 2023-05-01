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

#include "register_classes_components.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "scene/scene_container.hpp"
#include "scene/scene_manager.hpp"

#include "components/audio_listener_2d.hpp"
#include "components/audio_source_2d.hpp"
#include "components/camera_2d.hpp"
#include "components/canvas_item.hpp"
#include "components/canvas_layer.hpp"
#include "components/canvas_text.hpp"
#include "components/particles_2d.hpp"
#include "components/script_component.hpp"
#include "components/spatial_2d.hpp"
#include "components/sprite_instance.hpp"
#include "components/tag.hpp"

namespace wmoge {

#define WG_REGISTER_COMPONENT(component)                                                                         \
    void component::register_class() {                                                                           \
        auto* cls    = Class::register_class<component>();                                                       \
        auto* engine = Engine::instance();                                                                       \
        cls->set_instantiate([engine, cls]() { return engine->scene_manager()->get_container(cls)->create(); }); \
    }

    WG_REGISTER_COMPONENT(Spatial2d)
    WG_REGISTER_COMPONENT(CanvasLayer)
    WG_REGISTER_COMPONENT(CanvasItem)
    WG_REGISTER_COMPONENT(Camera2d)
    WG_REGISTER_COMPONENT(CanvasText)
    WG_REGISTER_COMPONENT(SpriteInstance)
    WG_REGISTER_COMPONENT(ScriptComponent)
    WG_REGISTER_COMPONENT(Particles2d)
    WG_REGISTER_COMPONENT(AudioSource2d)
    WG_REGISTER_COMPONENT(AudioListener2d)

    void register_classes_components() {
        Spatial2d::register_class();
        CanvasLayer::register_class();
        CanvasItem::register_class();
        Camera2d::register_class();
        CanvasText::register_class();
        SpriteInstance::register_class();
        ScriptComponent::register_class();
        Particles2d::register_class();
        AudioSource2d::register_class();
        AudioListener2d::register_class();
    }

}// namespace wmoge
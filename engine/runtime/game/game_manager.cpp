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

#include "game_manager.hpp"

#include "core/ioc_container.hpp"
#include "ecs/ecs_world.hpp"

#include "game/debug/components.hpp"
#include "game/render/components.hpp"
#include "game/transform/components.hpp"

#include "game/debug/features.hpp"
#include "game/transform/features.hpp"

namespace wmoge {

    GameManager::GameManager(EcsRegistry* ecs_registry, SceneManager* scene_manager) {
        m_ecs_registry  = ecs_registry;
        m_scene_manager = scene_manager;

        m_ecs_registry->register_component<GmParentComponent>("parent");
        m_ecs_registry->register_component<GmChildrenComponent>("children");
        m_ecs_registry->register_component<GmTransformComponent>("transform");
        m_ecs_registry->register_component<GmTransformFrameComponent>("transform_frame");
        m_ecs_registry->register_component<GmMatLocalComponent>("local");
        m_ecs_registry->register_component<GmMatLocalToWorldComponent>("local_to_world");
        m_ecs_registry->register_component<GmMatLocalToWorldPrevComponent>("local_to_world_prev");
        m_ecs_registry->register_component<GmMatWorldToLocalComponent>("world_to_local");

        m_ecs_registry->register_component<GmCameraComponent>("camera");
        m_ecs_registry->register_component<GmLightComponent>("light");
        m_ecs_registry->register_component<GmMeshComponent>("mesh");
        m_ecs_registry->register_component<GmModelComponent>("model");

        m_ecs_registry->register_component<GmDebugMeshComponent>("debug_shape");
        m_ecs_registry->register_component<GmDebugLabelComponent>("debug_label");
        m_ecs_registry->register_component<GmDebugPrimitiveComponent>("debug_primitive");

        m_scene_manager->add_trait(make_ref<WG_NAME_ENTITY_FEATURE_TRAIT(GmTransform)>());
        m_scene_manager->add_trait(make_ref<WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugMesh)>());
        m_scene_manager->add_trait(make_ref<WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugLabel)>());
        m_scene_manager->add_trait(make_ref<WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugPrimitive)>());
    }

    SceneRef GameManager::make_scene(const Strid& name) {
        SceneRef scene = m_scene_manager->make_scene(name);

        scene->add(std::make_shared<EcsWorld>(m_ecs_registry));

        return scene;
    }

    void bind_by_ioc_game_manager(class IocContainer* ioc) {
        ioc->bind_by_factory<GameManager>([ioc]() {
            return std::make_shared<GameManager>(
                    ioc->resolve_value<EcsRegistry>(),
                    ioc->resolve_value<SceneManager>());
        });
    }

}// namespace wmoge
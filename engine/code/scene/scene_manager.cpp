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

#include "scene_manager.hpp"

#include "core/ioc_container.hpp"
#include "core/task.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_world.hpp"
#include "profiler/profiler_cpu.hpp"
#include "scene/scene_uuid.hpp"

#include <cassert>
#include <utility>

namespace wmoge {

    void SceneManager::clear() {
        WG_PROFILE_CPU_SCENE("SceneManager::clear");

        for (auto& scene : m_scenes) {
            scene->finalize();
        }

        m_scenes.clear();
        m_to_clear.clear();
    }

    std::optional<SceneRef> SceneManager::find_scene_by_name(const Strid& name) {
        WG_PROFILE_CPU_SCENE("SceneManager::find_scene_by_name");

        for (SceneRef& scene : m_scenes) {
            if (scene->get_name() == name) {
                return scene;
            }
        }

        return std::nullopt;
    }

    SceneRef SceneManager::make_scene(const Strid& name) {
        WG_PROFILE_CPU_SCENE("SceneManager::make_scene");

        SceneCreateInfo info;
        info.name = name;
        return m_scenes.emplace_back(make_ref<Scene>(info));
    }

    Status SceneManager::build_scene(const SceneRef& scene, const SceneData& data) {
        WG_PROFILE_CPU_SCENE("SceneManager::build_scene");

        EcsWorld* world = scene->get<EcsWorld>();

        SceneUuidMap           uuid_map;
        std::vector<EcsEntity> entities;
        std::vector<EcsArch>   archs;

        EntitySetupContext setup_context;
        setup_context.scene = scene.get();
        setup_context.world = world;

        EntityBuildContext build_context;
        build_context.scene = scene.get();
        build_context.world = world;
        build_context.uuid  = &uuid_map;

        archs.reserve(data.entities.size());
        for (const EntityDesc& entity_desc : data.entities) {
            EcsArch entity_arch;

            for (const Ref<EntityFeature>& feature : entity_desc.features) {
                EntityFeatureTrait* trait = find_trait(feature->get_class_name()).value_or(nullptr);
                if (!trait) {
                    WG_LOG_ERROR("no such trait type for entity " << entity_desc.name << " feature " << feature->get_class_name());
                    return StatusCode::InvalidData;
                }

                EcsArch arch;
                if (!trait->setup_entity(arch, *feature, setup_context)) {
                    WG_LOG_ERROR("failed setup entity " << entity_desc.name << " feature " << feature->get_class_name());
                    return StatusCode::Error;
                }

                if ((arch & entity_arch).any() && false) {
                    WG_LOG_ERROR("feature arch collision for entity " << entity_desc.name << " feature " << feature->get_class_name());
                    return StatusCode::InvalidData;
                }

                entity_arch |= arch;
            }

            archs.push_back(entity_arch);
        }

        entities.reserve(archs.size());
        for (std::size_t i = 0; i < archs.size(); i++) {
            const EcsEntity entity = world->allocate_entity();

            entities.push_back(entity);
            world->make_entity(entity, archs[i]);
            uuid_map.add_entity(data.entities[i].uuid, entity);
        }

        for (std::size_t i = 0; i < entities.size(); i++) {
            const EcsEntity   entity      = entities[i];
            const EntityDesc& entity_desc = data.entities[i];

            for (const Ref<EntityFeature>& feature : entity_desc.features) {
                EntityFeatureTrait* trait = find_trait(feature->get_class_name()).value_or(nullptr);
                assert(trait);

                if (!trait->build_entity(entity, *feature, build_context)) {
                    WG_LOG_ERROR("failed build entity " << entity_desc.name << " feature " << feature->get_class_name());
                    return StatusCode::Error;
                }
            }
        }

        return WG_OK;
    }

    Async SceneManager::build_scene_async(TaskManager* task_manager, const SceneRef& scene, const Ref<SceneDataAsset>& data) {
        WG_PROFILE_CPU_SCENE("SceneManager::build_scene_async");

        Task task(scene->get_name(), [scene, data, this](TaskContext&) -> Status {
            if (!build_scene(scene, data->get_data())) {
                return StatusCode::Error;
            }
            return WG_OK;
        });

        return task.schedule(task_manager).as_async();
    }

    void SceneManager::add_trait(const Ref<EntityFeatureTrait>& trait) {
        m_traits[trait->get_feature_type()->get_name()] = trait;
    }

    std::optional<EntityFeatureTrait*> SceneManager::find_trait(const Strid& rtti) {
        auto iter = m_traits.find(rtti);
        return iter != m_traits.end() ? iter->second.get() : std::optional<EntityFeatureTrait*>();
    }

    void bind_by_ioc_scene_manager(class IocContainer* ioc) {
        ioc->bind<SceneManager>();
    }

}// namespace wmoge
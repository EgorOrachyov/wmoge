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

#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_system.hpp"
#include "ecs/ecs_world.hpp"
#include "profiler/profiler.hpp"
#include "scene/scene_components.hpp"

#include <atomic>

namespace wmoge {

    /**
     * @class EcsSysUpdateHier
     * @brief System to iteratively update spatial hierarchy of transforms
    */
    class EcsSysUpdateHier : public EcsSystem {
    public:
        WG_ECS_SYSTEM(EcsSysUpdateHier, Update, WorkerThreads);

        void process(EcsWorld&                    world,
                     const EcsEntity&             entity,
                     const EcsComponentParent&    parent,
                     const EcsComponentChildren&  children,
                     const EcsComponentTransform& transform,
                     EcsComponentTransformUpd&    transform_upd,
                     EcsComponentLocalToWorld&    l2w,
                     EcsComponentWorldToLocal&    w2l);

        std::atomic<int> num_updated{0};
        std::atomic<int> num_dirty{0};
        int              current_batch = 0;
        int              frame_id      = -1;
    };

    /**
     * @class EcsSysUpdateCameras
     * @brief Updates cameras based on transform
    */
    class EcsSysUpdateCameras : public EcsSystem {
    public:
        WG_ECS_SYSTEM(EcsSysUpdateCameras, Update, WorkerThreads);

        void process(EcsWorld&                       world,
                     const EcsEntity&                entity,
                     const EcsComponentTransformUpd& transform_upd,
                     const EcsComponentLocalToWorld& l2w,
                     EcsComponentCamera&             camera);

        int frame_id = -1;
    };

    /**
     * @class EcsSysUpdateAabb
     * @brief Updates bounding boxes based on transform
    */
    class EcsSysUpdateAabb : public EcsSystem {
    public:
        WG_ECS_SYSTEM(EcsSysUpdateAabb, Update, WorkerThreads);

        void process(EcsWorld&                       world,
                     const EcsEntity&                entity,
                     const EcsComponentTransformUpd& transform_upd,
                     const EcsComponentLocalToWorld& l2w,
                     const EcsComponentAabbLocal&    bbox_local,
                     EcsComponentAabbWorld&          bbox_world);

        int frame_id = -1;
    };

    /**
     * @class EcsSysReleaseCullItem
     * @brief System to delete cull items
    */
    class EcsSysReleaseCullItem : public EcsSystem {
    public:
        WG_ECS_SYSTEM(EcsSysReleaseCullItem, Destroy, SingleThread);

        void process(EcsWorld&                world,
                     const EcsEntity&         entity,
                     EcsComponentCullingItem& culling_item);
        ;
    };

}// namespace wmoge
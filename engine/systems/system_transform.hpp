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

#ifndef WMOGE_SYSTEM_TRANSFORM_HPP
#define WMOGE_SYSTEM_TRANSFORM_HPP

#include "ecs/ecs_system.hpp"
#include "ecs/ecs_world.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_transform.hpp"

namespace wmoge {

    /**
     * @class EcsSysCacheMatrices
     * @brief Store cached matrices after hier update for each entity
     */
    class EcsSysCacheMatrices final : public EcsSystem {
    public:
        void process(EcsWorld& world, const EcsEntity& enity,
                     EcsComponentSceneTransform& st,
                     EcsComponentLocalToWorld&   l2w,
                     EcsComponentLocalToParent&  l2p) {
            l2w.matrix = st.transform->get_l2w_cached();
            l2p.matrix = st.transform->get_lt();
        }

        void process_batch(class EcsWorld& world, EcsArchStorage& storage, int start_entity, int count) override {
            WG_ECS_SYSTEM_BIND(EcsSysCacheMatrices);
        }

        EcsSystemExecMode get_exec_mode() const override { return EcsSystemExecMode::OnWorkers; }
        StringId          get_name() const override { return SID("cache_matrices"); }
        EcsQuery          get_query() const override {
            EcsQuery query;
            query.set_read<EcsComponentSceneTransform>();
            query.set_write<EcsComponentLocalToWorld>();
            query.set_write<EcsComponentLocalToParent>();
            return query;
        }
    };

}// namespace wmoge

#endif//WMOGE_SYSTEM_TRANSFORM_HPP
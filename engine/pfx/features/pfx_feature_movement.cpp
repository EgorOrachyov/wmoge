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

#include "pfx_feature_movement.hpp"

#include "debug/profiler.hpp"
#include "pfx/pfx_component_runtime.hpp"

namespace wmoge {

    ref_ptr<PfxFeature> PfxFeatureMovement2d::create() const {
        return make_ref<PfxFeatureMovement2d>();
    }
    StringId PfxFeatureMovement2d::get_feature_name() const {
        static StringId name("Movement2d");
        return name;
    }
    StringId PfxFeatureMovement2d::get_feature_family() const {
        static StringId family("2d");
        return family;
    }
    bool PfxFeatureMovement2d::load_from_options(const YamlConstNodeRef& node) {
        Yaml::read(node["speed_acceleration"], m_speed_acceleration);
        node["angle_acceleration"] >> m_angle_acceleration;
        return true;
    }
    void PfxFeatureMovement2d::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::Pos2d);
        attributes.set(PfxAttribute::Vel2d);
        attributes.set(PfxAttribute::Angle);
    }
    void PfxFeatureMovement2d::on_update(class PfxComponentRuntime& runtime, float dt) {
        WG_AUTO_PROFILE_PFX();

        auto* storage = runtime.get_storage();
        auto  view_p  = storage->get_pos2d();
        auto  view_v  = storage->get_vel2d();
        auto  view_a  = storage->get_angle();

        for (auto particle_id : runtime.get_update_range()) {
            view_p[particle_id] += view_v[particle_id] * dt;
            view_v[particle_id] += m_speed_acceleration * dt;
            view_a[particle_id] += m_angle_acceleration * dt;
        }
    }

    void PfxFeatureMovement2d::register_class() {
        auto* cls = Class::register_class<PfxFeatureMovement2d>();
    }

}// namespace wmoge
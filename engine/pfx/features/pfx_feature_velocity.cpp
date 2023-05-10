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

#include "pfx_feature_velocity.hpp"

#include "core/random.hpp"
#include "debug/profiler.hpp"
#include "pfx/pfx_component_runtime.hpp"

namespace wmoge {

    ref_ptr<PfxFeature> PfxFeatureVelocity2d::create() const {
        return make_ref<PfxFeatureVelocity2d>();
    }
    StringId PfxFeatureVelocity2d::get_feature_name() const {
        static StringId name("Velocity2d");
        return name;
    }
    StringId PfxFeatureVelocity2d::get_feature_family() const {
        static StringId family("2d");
        return family;
    }
    bool PfxFeatureVelocity2d::load_from_options(const YamlConstNodeRef& node) {
        node["radius"] >> m_radius;
        return true;
    }
    void PfxFeatureVelocity2d::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::Vel2d);
    }
    void PfxFeatureVelocity2d::on_spawn(class PfxComponentRuntime& runtime, const PfxSpawnParams& params) {
        WG_AUTO_PROFILE_PFX("PfxFeatureVelocity2d::on_spawn");

        auto* storage = runtime.get_storage();
        auto  view_v  = storage->get_vel2d();

        for (auto particle_id : runtime.get_spawn_range()) {
            float rnd_x = Random::next_float_in_range(-1.0f, 1.0f);
            float rnd_y = Random::next_float_in_range(-1.0f, 1.0f);
            view_v[particle_id] += Vec2f(rnd_x, rnd_y).normalized() * m_radius;
        }
    }

    void PfxFeatureVelocity2d::register_class() {
        auto* cls = Class::register_class<PfxFeatureVelocity2d>();
    }

}// namespace wmoge
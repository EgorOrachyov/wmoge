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

#include "pfx_feature_lifetime.hpp"

#include "debug/profiler.hpp"
#include "math/math_utils.hpp"
#include "pfx/pfx_component_runtime.hpp"

#include <limits>

namespace wmoge {

    ref_ptr<PfxFeature> PfxFeatureLifetime::create() const {
        return make_ref<PfxFeatureLifetime>();
    }
    StringId PfxFeatureLifetime::get_feature_name() const {
        static StringId name("Lifetime");
        return name;
    }
    StringId PfxFeatureLifetime::get_feature_family() const {
        static StringId family("Time");
        return family;
    }
    bool PfxFeatureLifetime::load_from_options(const YamlConstNodeRef& node) {
        node["lifetime"] >> m_lifetime;
        node["infinite"] >> m_infinite;
        return true;
    }
    void PfxFeatureLifetime::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::Time);
        attributes.set(PfxAttribute::TimeNorm);
        attributes.set(PfxAttribute::TimeToLive);
    }
    void PfxFeatureLifetime::on_spawn(class PfxComponentRuntime& runtime, const PfxSpawnParams& params) {
        WG_AUTO_PROFILE_PFX();

        const float time_to_set = m_infinite ? std::numeric_limits<float>::max() : m_lifetime;

        auto* storage  = runtime.get_storage();
        auto  view_t   = storage->get_time();
        auto  view_tn  = storage->get_time_norm();
        auto  view_ttl = storage->get_time_to_live();

        for (auto particle_id : runtime.get_spawn_range()) {
            view_t[particle_id]   = 0.0f;
            view_tn[particle_id]  = 0.0f;
            view_ttl[particle_id] = time_to_set;
        }
    }

    void PfxFeatureLifetime::on_update(class PfxComponentRuntime& runtime, float dt) {
        WG_AUTO_PROFILE_PFX();

        auto* storage  = runtime.get_storage();
        auto  view_t   = storage->get_time();
        auto  view_tn  = storage->get_time_norm();
        auto  view_ttl = storage->get_time_to_live();

        for (auto particle_id : runtime.get_update_range()) {
            view_t[particle_id] += dt;
            view_tn[particle_id] = Math::min(view_t[particle_id] / view_ttl[particle_id], 1.0f);
        }
    }

    void PfxFeatureLifetime::register_class() {
        auto* cls = Class::register_class<PfxFeatureLifetime>();
    }

}// namespace wmoge
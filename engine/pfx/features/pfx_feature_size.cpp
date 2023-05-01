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

#include "pfx_feature_size.hpp"

#include "debug/profiler.hpp"
#include "pfx/pfx_component_runtime.hpp"

namespace wmoge {

    ref_ptr<PfxFeature> PfxFeatureSize::create() const {
        return make_ref<PfxFeatureSize>();
    }
    StringId PfxFeatureSize::get_feature_name() const {
        static StringId name("Size");
        return name;
    }
    StringId PfxFeatureSize::get_feature_family() const {
        static StringId family("Visual");
        return family;
    }
    bool PfxFeatureSize::load_from_options(const YamlConstNodeRef& node) {
        node["start_size"] >> m_start_size;
        node["end_size"] >> m_end_size;
        return true;
    }
    void PfxFeatureSize::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::Size);
        attributes.set(PfxAttribute::TimeNorm);
    }
    void PfxFeatureSize::on_spawn(class PfxComponentRuntime& runtime, const struct PfxSpawnParams& params) {
        WG_AUTO_PROFILE_PFX();

        auto* storage = runtime.get_storage();
        auto  view_s  = storage->get_size();

        for (auto particle_id : runtime.get_spawn_range()) {
            view_s[particle_id] = m_start_size;
        }
    }
    void PfxFeatureSize::on_update(class PfxComponentRuntime& runtime, float dt) {
        WG_AUTO_PROFILE_PFX();

        auto* storage = runtime.get_storage();
        auto  view_s  = storage->get_size();
        auto  view_t  = storage->get_time_norm();

        for (auto particle_id : runtime.get_update_range()) {
            view_s[particle_id] = Math::lerp(view_t[particle_id], m_start_size, m_end_size);
        }
    }

    void PfxFeatureSize::register_class() {
        auto* cls = Class::register_class<PfxFeatureSize>();
    }

}// namespace wmoge
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

#include "pfx_feature_color.hpp"

#include "debug/profiler.hpp"
#include "math/math_utils.hpp"
#include "pfx/pfx_component_runtime.hpp"

namespace wmoge {

    ref_ptr<PfxFeature> PfxFeatureColor::create() const {
        return make_ref<PfxFeatureColor>();
    }
    StringId PfxFeatureColor::get_feature_name() const {
        static StringId name("Color");
        return name;
    }
    StringId PfxFeatureColor::get_feature_family() const {
        static StringId family("Visual");
        return family;
    }
    bool PfxFeatureColor::load_from_options(const YamlConstNodeRef& node) {
        Yaml::read(node["start_color"], m_start_color);
        Yaml::read(node["end_color"], m_end_color);
        return true;
    }
    void PfxFeatureColor::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::TimeNorm);
        attributes.set(PfxAttribute::Color);
    }
    void PfxFeatureColor::on_spawn(class PfxComponentRuntime& runtime, const PfxSpawnParams& params) {
        WG_AUTO_PROFILE_PFX("PfxFeatureColor::on_spawn");

        auto* storage    = runtime.get_storage();
        auto  view_color = storage->get_color();

        for (auto particle_id : runtime.get_spawn_range()) {
            view_color[particle_id] = m_start_color;
        }
    }
    void PfxFeatureColor::on_update(class PfxComponentRuntime& runtime, float dt) {
        WG_AUTO_PROFILE_PFX("PfxFeatureColor::on_update");

        auto* storage    = runtime.get_storage();
        auto  view_color = storage->get_color();
        auto  view_tn    = storage->get_time_norm();

        for (auto particle_id : runtime.get_update_range()) {
            view_color[particle_id] = Vec4f::lerp(view_tn[particle_id], m_start_color, m_end_color);
        }
    }

    void PfxFeatureColor::register_class() {
        auto* cls = Class::register_class<PfxFeatureColor>();
    }

}// namespace wmoge
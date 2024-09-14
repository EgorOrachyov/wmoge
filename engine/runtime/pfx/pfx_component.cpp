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

#include "pfx_component.hpp"

#include "core/class.hpp"
#include "core/log.hpp"

#include <cassert>

namespace wmoge {

    void PfxComponent::add_feature(Ref<PfxFeature> feature) {
        assert(feature);
        m_features.push_back(feature);
        feature->on_added(m_attributes);
    }
    void PfxComponent::set_amount(int amount) {
        assert(amount);
        m_amount = amount;
    }
    void PfxComponent::set_active(bool active) {
        m_active = active;
    }
    const Ref<PfxFeature>& PfxComponent::get_feature(int id) const {
        assert(id < m_features.size());
        return m_features[id];
    }
    PfxAttributes PfxComponent::get_attributes() const {
        return m_attributes;
    }
    const Strid& PfxComponent::get_name() const {
        return m_name;
    }
    int PfxComponent::get_features_count() const {
        return int(m_features.size());
    }
    int PfxComponent::get_amount() const {
        return m_amount;
    }
    bool PfxComponent::is_active() const {
        return m_active;
    }

}// namespace wmoge
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

#include "pfx_effect.hpp"

#include "core/class.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    void PfxEffect::copy_to(Resource& copy) {
        Resource::copy_to(copy);
    }

    void PfxEffect::add_component(Ref<PfxComponent> component) {
        assert(component);
        m_components.push_back(std::move(component));
    }
    const Ref<PfxComponent>& PfxEffect::get_component(int id) const {
        assert(id < m_components.size());
        return m_components[id];
    }
    int PfxEffect::get_components_count() const {
        return int(m_components.size());
    }
    PfxEffectType PfxEffect::get_type() const {
        return m_type;
    }

    void PfxEffect::register_class() {
        auto* cls = Class::register_class<PfxEffect>();
    }

}// namespace wmoge

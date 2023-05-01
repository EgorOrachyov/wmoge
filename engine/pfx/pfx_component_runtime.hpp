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

#ifndef WMOGE_PFX_COMPONENT_RUNTIME_HPP
#define WMOGE_PFX_COMPONENT_RUNTIME_HPP

#include "core/data.hpp"
#include "math/aabb.hpp"
#include "pfx/pfx_component.hpp"
#include "pfx/pfx_renderer.hpp"
#include "pfx/pfx_storage.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class PfxComponentRuntime
     * @brief Runtime representation of a spawned active pfx component
     */
    class PfxComponentRuntime final {
    public:
        PfxComponentRuntime(class PfxEmitter* emitter, class PfxComponent* component);

        void emit(const PfxSpawnParams& params);
        void update(float dt);
        void render(RenderViewList& views, const RenderViewMask& mask, RenderObject* object);

        PfxRange            get_update_range();
        PfxRange            get_spawn_range();
        PfxRange            get_full_range();
        PfxStorage*         get_storage();
        PfxRenderer*        get_renderer();
        Aabbf&              get_bounds();
        class PfxEmitter*   get_emitter();
        class PfxComponent* get_component();
        bool                get_is_active() const;
        int                 get_active_amount() const;
        int                 get_spawned_amount() const;

    private:
        std::unique_ptr<PfxStorage>  m_storage;
        std::unique_ptr<PfxRenderer> m_renderer;
        class PfxEmitter*            m_emitter;
        class PfxComponent*          m_component;
        Aabbf                        m_bounds;
        int                          m_active_amount  = 0;
        int                          m_active_from    = 0;
        int                          m_spawned_amount = 0;
        int                          m_spawned_from   = 0;
        bool                         m_is_active      = false;
    };

}// namespace wmoge

#endif//WMOGE_PFX_COMPONENT_RUNTIME_HPP

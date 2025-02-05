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

#include "weak_ref.hpp"

namespace wmoge {

    bool WeakRefAccess::try_release_object() {
        std::lock_guard guard(m_mutex);

        if (!m_object) {
            // Already released, it is ok
            // since it is possible that a-bb-a (2 shared ptr) reached 0 at some point
            // due to b constucted from weak ref
            return false;
        }

        if (m_object->refs_count() == 0) {
            // It is actually was last ref to object in some shared prt
            // and now there no more refs, we must destory object here.
            // Note: it is safe, since we are under mutex now, only here weak prts can add refs
            m_object = nullptr;
            return true;
        }

        return false;
    }

    std::optional<Ref<RefCnt>> WeakRefAccess::try_acquire_object() {
        std::lock_guard guard(m_mutex);

        if (m_object) {
            // Object is alive, construct shared ref under mutex
            Ref<RefCnt> ptr(m_object);
            return std::optional<Ref<RefCnt>>(std::move(ptr));
        }

        return std::nullopt;
    }

}// namespace wmoge
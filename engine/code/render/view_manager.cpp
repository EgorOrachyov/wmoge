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

#include "view_manager.hpp"

#include <algorithm>
#include <cassert>

namespace wmoge {

    ViewManager::ViewManager() {
        m_active = create_view(SID("default"));
    }

    Ref<View> ViewManager::create_view(Strid name) {
        auto view = make_ref<View>(std::move(name));
        m_views.push_back(view);
        return view;
    }

    void ViewManager::delete_view(const Ref<View>& view) {
        assert(has_view(view));
        m_views.erase(std::find(m_views.begin(), m_views.end(), view));
    }

    void ViewManager::make_active(Ref<View> view) {
        m_active = std::move(view);
    }

    bool ViewManager::has_view(const Ref<View>& view) {
        return std::find(m_views.begin(), m_views.end(), view) != m_views.end();
    }

}// namespace wmoge
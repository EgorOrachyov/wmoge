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

#ifndef WMOGE_SCRIPT_MANAGER_HPP
#define WMOGE_SCRIPT_MANAGER_HPP

#include "core/string_id.hpp"

namespace wmoge {

    /**
     * @class ScriptSystem
     * @brief Engine script system responsible for scripting features for a game
     *
     * Scripting system allows to attach scripts to any engine objects to extend
     * their logic, get notifications on events, create objects from scripting,
     * manage scene, process events, updates, etc.
     *
     * Scripting system takes responsibility on managing objects lifetime,
     * scripts and class loading, global states, and error tracking.
     *
     * From a user perspective it is enough to create script and attach it to
     * a scene object. The engine will do the rest.
     *
     * @see Script
     * @see ScriptInstance
     * @see ScriptContainer
     */
    class ScriptSystem {
    public:
        virtual ~ScriptSystem() = default;

        virtual void gc()       = 0;
        virtual void update()   = 0;
        virtual void shutdown() = 0;

        const StringId& get_name() { return m_name; }
        const StringId& get_language() { return m_language; }

    protected:
        StringId    m_name                = SID("none");
        StringId    m_language            = SID("none");
        std::size_t m_gc_interval         = 15;
        std::size_t m_gc_cycles           = 0;
        std::size_t m_gc_frames_from_last = 0;
    };

}// namespace wmoge

#endif//WMOGE_SCRIPT_MANAGER_HPP

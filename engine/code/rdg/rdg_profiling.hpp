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

#pragma once

#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

namespace wmoge {

    /** @brief Rdg mark for graph profiling */
    struct RdgProfileMark {
        RdgProfileMark(std::string name,
                       Strid       category,
                       Strid       function,
                       Strid       file,
                       std::size_t line);

        ProfilerCpuMark mark_cpu;
        ProfilerGpuMark mark_gpu;
    };

    /** @brief Rdg scope for graph profiling */
    struct RdgProfileScope {
        RdgProfileScope(RdgProfileMark& mark, const std::string& data, class RdgGraph& graph);
        ~RdgProfileScope();

        class RdgGraph& graph;
    };

}// namespace wmoge

#define WG_PROFILE_RDG_MARK(var, system, name)                                                      \
    static RdgProfileMark var {                                                                     \
        std::string(name), SID(#system), SID(__FUNCTION__), SID(__FILE__), std::size_t { __LINE__ } \
    }

#define WG_PROFILE_RDG_SCOPE_WITH_DESC(name, graph, system, desc) \
    WG_PROFILE_RDG_MARK(__wg_auto_mark_rdg, system, name);        \
    RdgProfileScope __wg_auto_scope_gpu(__wg_auto_mark_rdg, desc, graph)

#define WG_PROFILE_RDG_SCOPE(name, graph) \
    WG_PROFILE_RDG_SCOPE_WITH_DESC(name, graph, gpurdg, "")
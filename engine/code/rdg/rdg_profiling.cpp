#include "rdg_profiling.hpp"

#include "rdg/rdg_graph.hpp"

namespace wmoge {

    RdgProfileScope::RdgProfileScope(RdgProfileMark& mark, const std::string& data, RdgGraph& graph) : graph(graph) {
        graph.push_event(&mark, data);
    }

    RdgProfileScope::~RdgProfileScope() {
        graph.pop_event();
    }

    RdgProfileMark::RdgProfileMark(std::string name, Strid category, Strid function, Strid file, std::size_t line) {
        mark_cpu = std::move(ProfilerCpuMark{name, category, function, file, line});
        mark_gpu = std::move(ProfilerGpuMark{name, category, function, file, line});
    }

}// namespace wmoge
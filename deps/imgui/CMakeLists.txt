add_library(imgui INTERFACE imgui.h imgui.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_vulkan.cpp)
target_include_directories(imgui INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
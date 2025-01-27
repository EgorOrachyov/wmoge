add_library(imgui STATIC imgui.h imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_vulkan.cpp)
target_link_libraries(imgui PUBLIC glfw volk)
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK)
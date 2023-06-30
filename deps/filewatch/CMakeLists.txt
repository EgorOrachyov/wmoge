add_library(filewatch INTERFACE FileWatch.hpp)
target_include_directories(filewatch INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
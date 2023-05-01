add_library(lz4 STATIC lz4.hpp lz4.cpp lz4hc.hpp lz4hc.cpp)
target_include_directories(lz4 PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
set(CMAKE_CXX_STANDARD 17)
add_library(base64 STATIC base64.h base64.cpp)
target_include_directories(base64 PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
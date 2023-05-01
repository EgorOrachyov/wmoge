add_library(stbimage STATIC stb_image.hpp stb_image_resize.hpp stb_image_write.hpp stb_image.cpp)
target_include_directories(stbimage PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
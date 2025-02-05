#===============================================================================
cmake_minimum_required (VERSION 3.12)

project ("AudioFile" VERSION 1.1.0
                     DESCRIPTION "A simple C++ library for reading and writing audio files."
                     HOMEPAGE_URL "https://github.com/adamstark/AudioFile")

set(TARGET_NAME audio_file)

#===============================================================================
add_library (${TARGET_NAME} INTERFACE)

#===============================================================================
if(MSVC)
  # needed for M_PI macro
  add_definitions(-D_USE_MATH_DEFINES)
endif()

#===============================================================================
target_include_directories (
  ${TARGET_NAME}
  INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

#===============================================================================
target_compile_features (${TARGET_NAME} INTERFACE cxx_std_11)

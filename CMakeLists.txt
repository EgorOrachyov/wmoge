cmake_minimum_required(VERSION 3.0)
project(wmoge)

###########################################################
# global cxx setup
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###########################################################
# Define platform
#  - WINDOWS = Windows Desktop
#  - MACOSX  = MacOS X
#  - LINUX   = Linux

set(TARGET_WINDOWS NO)
set(TARGET_LINUX NO)
set(TARGET_MACOS NO)
set(TARGET_DEFINES)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(TARGET_WINDOWS YES)
    list(APPEND TARGET_DEFINES TARGET_WINDOWS)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(TARGET_LINUX YES)
    list(APPEND TARGET_DEFINES TARGET_LINUX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(TARGET_MACOS YES)
    list(APPEND TARGET_DEFINES TARGET_MACOS)
else ()
    message(FATAL_ERROR "Unsupported target platform")
endif ()

###########################################################
# Define build type

if (CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND TARGET_DEFINES WG_DEBUG)
    message(STATUS "Build project in debug mode (specified)")
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND TARGET_DEFINES WG_RELEASE)
    message(STATUS "Build project in release mode (specified)")
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    list(APPEND TARGET_DEFINES WG_DEBUG_RELEASE)
    message(STATUS "Build project in release mode with debug info (specified)")
else ()
    list(APPEND TARGET_DEFINES WG_RELEASE)
    message(STATUS "Build project in release mode (default, not specified)")
endif ()

###########################################################
# Aux functions to work with targets

function(wmoge_target_defs target)
    foreach (DEFINITION ${TARGET_DEFINES})
        target_compile_definitions(${target} PUBLIC ${DEFINITION})
    endforeach ()
endfunction()

function(wmoge_game name sources icon)
    # game executable (with optional executable icon)
    add_executable(${name} ${sources} ${icon})
    # link engine library to our game
    target_link_libraries(${name} PRIVATE wmoge)
endfunction()

###########################################################
# Third-party dependencies of the tutorial

# Fast hash map
add_subdirectory(deps/robin_hood)

# Fast vector with SVO optimization
add_subdirectory(deps/svector)

# Command-line options parses
add_subdirectory(deps/cxxopts)

# Static enum reflection
add_subdirectory(deps/magic_enum)

# Executable file location
add_subdirectory(deps/whereami)

# Image manipulations
add_subdirectory(deps/stbimage)

# File system events tracking
add_subdirectory(deps/filewatch)

# Add zlib as auxiliary (for freetype and assimp usage)
add_subdirectory(deps/zlib)
set(ZLIB_FOUND TRUE)
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/deps/zlib")
set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})

# lz4 fast decompression and compression library
add_subdirectory(deps/lz4)

# Xml files parser
add_subdirectory(deps/tinyxml2)

# Yaml files parser
set(RYML_WITH_TAB_TOKENS OFF CACHE BOOL "" FORCE)
set(RYML_DEFAULT_CALLBACKS ON CACHE BOOL "" FORCE)
set(RYML_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(RYML_BUILD_API OFF CACHE BOOL "" FORCE)
set(RYML_DBG OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/rapidyaml)

# Glfw for cross-platform window management
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)

# Vulkan runtime functions loader
add_subdirectory(deps/volk)

# Vulkan memory allocator for gpu memory management
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE deps/vma/include)

# Glslang for runtime glsl to spir-v compilation and reflection
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_JS OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glslang)

# Assimp for runtime geometry, mesh, animation import
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FRAMEWORK OFF CACHE BOOL "" FORCE)
set(ASSIMP_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(ASSIMP_OPT_BUILD_PACKAGES OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_COVERALLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS ON CACHE BOOL "" FORCE)
set(ASSIMP_ASAN OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_IGNORE_GIT_HASH OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/assimp)

# Freetype for font glyphs loading and rendering
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_HEADERS ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_LIBRARIES ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
add_subdirectory(deps/freetype)

# Compression tool (for texture compression primary)
set(OPTION_ENABLE_ALL_APPS OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/compressonator/library)

# Lua deps for scripting
add_subdirectory(deps/lua)
add_subdirectory(deps/luabridge)

# Audio file small library for WAV loading
add_subdirectory(deps/audio_file)

# OpenAL software implementation for low-level audion rendering
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_UPDATE_BUILD_VERSION OFF CACHE BOOL "" FORCE)
set(LIBTYPE "STATIC" CACHE STRING "" FORCE)
add_subdirectory(deps/openal_soft)

###########################################################
# Engine sources (will be as static library)

add_subdirectory(engine)

###########################################################
# Game template for test purposes and to kick-off new projects

add_subdirectory(template)

###########################################################
# Engine example game projects

# add_subdirectory(games)
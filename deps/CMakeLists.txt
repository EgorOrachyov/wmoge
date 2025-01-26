# Fast hash map
add_subdirectory(robin_hood)

# Fast vector with SVO optimization
add_subdirectory(svector)

# Command-line options parses
add_subdirectory(cxxopts)

# Cross-platfrom dynamic and shared library loader
add_subdirectory(dynalo)

# Base64 encoding/decoding library
add_subdirectory(base64)

# Sha25 hashing
add_subdirectory(sha256)

# Static enum reflection
add_subdirectory(magic_enum)

# For fast MT sync primitives
add_subdirectory(yamc)

# Executable file location
add_subdirectory(whereami)

# Image manipulations
add_subdirectory(stbimage)

# File system events tracking
add_subdirectory(filewatch)

# Add zlib as auxiliary (for freetype and assimp usage)
add_subdirectory(zlib)
set(ZLIB_FOUND TRUE)
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/zlib")
set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})

# Lz4 fast decompression and compression library
add_subdirectory(lz4)

# Xml files parser
add_subdirectory(tinyxml2)

# Yaml files parser
set(RYML_WITH_TAB_TOKENS OFF CACHE BOOL "" FORCE)
set(RYML_DEFAULT_CALLBACKS ON CACHE BOOL "" FORCE)
set(RYML_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(RYML_BUILD_API OFF CACHE BOOL "" FORCE)
set(RYML_DBG OFF CACHE BOOL "" FORCE)
add_subdirectory(rapidyaml)

# Glfw for cross-platform window management
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# Vulkan runtime functions loader
add_subdirectory(volk)

# Vulkan memory allocator for gpu memory management
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vma/include)

# Glslang for runtime glsl to spir-v compilation and reflection
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_JS OFF CACHE BOOL "" FORCE)
add_subdirectory(glslang)

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
add_subdirectory(assimp)

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
add_subdirectory(freetype)

# Compression tool (for texture compression primary)
set(OPTION_ENABLE_ALL_APPS OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_CMP_SDK ON CACHE BOOL "" FORCE)
add_subdirectory(compressonator)
add_library(compressonator INTERFACE)
target_include_directories(compressonator INTERFACE compressonator/cmp_compressonatorlib)
target_link_libraries(compressonator INTERFACE CMP_Compressonator)

# Lua deps for scripting
add_subdirectory(lua)
add_subdirectory(luabridge)

# Audio file small library for WAV loading
add_subdirectory(audio_file)

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
add_subdirectory(openal_soft)

# ImGui for ui backend
add_subdirectory(imgui)
cmake_minimum_required(VERSION 3.10)

# Cmake build examples: for Visual Studio 2017
#
# cmake -G "Visual Studio 15 2017 Win64"   Enable building all apps and libs
# cmake -DOPTION_ENABLE_ALL_APPS=OFF -G "Visual Studio 15 2017 Win64"    Disable all builds except external libs, minimal cmake base setup 
# cmake -DOPTION_ENABLE_ALL_APPS=OFF -DOPTION_BUILD_APPS_CMP_CLI=ON -G "Visual Studio 15 2017 Win64"   Enable only CLI app build
# cmake -DOPTION_ENABLE_ALL_APPS=OFF -DOPTION_BUILD_APPS_CMP_GUI=ON -G "Visual Studio 15 2017 Win64"   Enable only GUI app build
# cmake -DOPTION_ENABLE_ALL_APPS=OFF -DOPTION_BUILD_APPS_TESTCORE=ON -G "Visual Studio 15 2017 Win64"  Enable only Test Core App build
#
# Other options are available : see OPTION_BUILDS... 
#

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif ()

if (POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif ()

project(Compressonator)

# ------------------------------
# Helper function for the build 
# ------------------------------
include(cmake/helperfunctions.cmake)


#----------------------------------------
# Check compiler feature support levels
# for Apple (Mac) default to C++11
#---------------------------------------
if (CMP_HOST_LINUX OR CMP_HOST_WINDOWS)
    foreach (i ${CMAKE_CXX_COMPILE_FEATURES})
        if ("${i}" STREQUAL "cxx_std_11")
            set(COMPILER_SUPPORTS_CXX11 ON)
            message("Complier Supports ${i}")
        endif ()
        if ("${i}" STREQUAL "cxx_std_14")
            set(COMPILER_SUPPORTS_CXX14 ON)
            message("Complier Supports ${i}")
        endif ()
        if ("${i}" STREQUAL "cxx_std_17")
            set(COMPILER_SUPPORTS_CXX17 ON)
            message("Complier Supports ${i}")
        endif ()
        # message("${i}")
    endforeach ()
else ()
    message("UNIX Build: CHECK_CXX_COMPILER_FLAG defaulted to -std=c++11")
    set(COMPILER_SUPPORTS_CXX11 ON)
endif ()

option(OPTION_ENABLE_ALL_APPS "Enable all apps" ON)

#--------------------------------------------------------------------------
# Enable or Disable Specific Build Apps
#--------------------------------------------------------------------------
if (OPTION_ENABLE_ALL_APPS)
    set(OPTION_BUILD_APPS_CMP_CLI "Build Application: compressonatorcli" ON)
    set(OPTION_BUILD_APPS_CMP_GUI "Build Application: compressonator gui" ON)
    set(OPTION_BUILD_APPS_CMP_TESTCORE "Build Application: cmp_testcore" ON)
    set(OPTION_BUILD_APPS_CMP_EXAMPLES "Build Application: examples" ON)
    set(OPTION_BUILD_APPS_CMP_CAS "Build Application: cmp_cas" ON)
else ()
    option(OPTION_BUILD_APPS_CMP_CLI OFF)
    option(OPTION_BUILD_APPS_CMP_GUI OFF)
    option(OPTION_BUILD_APPS_CMP_TESTCORE OFF)
    option(OPTION_BUILD_APPS_CMP_EXAMPLES OFF)
    option(OPTION_BUILD_APPS_CMP_CAS "Build Application: cmp_cas" OFF)
endif ()

# Minimum Lib Dependencies for CLI and GUI (GUI has addtion lib requirements condition later in this cmake)
if (OPTION_BUILD_APPS_CMP_CLI OR OPTION_BUILD_APPS_CMP_GUI)
    set(LIB_BUILD_COMPRESSONATOR_SDK ON)
    set(LIB_BUILD_FRAMEWORK_SDK ON)
    set(LIB_BUILD_IMAGEIO ON)
    set(LIB_BUILD_ANALYSIS ON)
    set(LIB_BUILD_COMMON ON)
    set(LIB_BUILD_CORE ON)
    set(LIB_BUILD_GPUDECODE ON)
endif ()

if (OPTION_BUILD_APPS_CMP_VISION)
    set(LIB_BUILD_COMPRESSONATOR_SDK ON)
    set(LIB_BUILD_FRAMEWORK_SDK ON)
endif ()

# The following options that have been removed
set(LIB_BUILD_MESHCOMPRESSOR OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported configuration options" FORCE)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build-out")

set(PROJECT_EXTERNAL_LIBDIR ${PROJECT_SOURCE_DIR}/../common/lib/ext)

# ----------------------------------------------------------------------------
# GPUOpen Source Code build options : These options are been revised for v4.2
# ----------------------------------------------------------------------------
if (${CMAKE_VERSION} VERSION_GREATER "3.14.0")
    cmp_option(OPTION_BUILD_KTX2 "Build CLI KTX2 Support" ON)
else ()
    cmp_option(OPTION_BUILD_KTX2 "Build CLI KTX2 Support" OFF)
endif ()
cmp_option(OPTION_BUILD_EXR "Build CLI EXR Support" CMP_HOST_WINDOWS OR CMP_HOST_LINUX)
cmp_option(OPTION_BUILD_DRACO "Build CLI Draco Support" OFF)
cmp_option(OPTION_BUILD_GUI "Build the GUI Application" CMP_HOST_WINDOWS OR OPTION_BUILD_APPS_CMP_GUI)
cmp_option(OPTION_CMP_DIRECTX "Use Directx" CMP_HOST_WINDOWS) # set for windows only
cmp_option(OPTION_CMP_VULKAN "Use Vulkan" OPTION_ENABLE_ALL_APPS)
cmp_option(OPTION_CMP_OPENGL "Use OpenGL" ON)   # available on all platforms
cmp_option(OPTION_CMP_QT "Use Qt for Image Loading" OPTION_ENABLE_ALL_APPS OR OPTION_BUILD_APPS_CMP_GUI)
cmp_option(OPTION_CMP_OPENCV "Use OpenCV" ON)   # available on all platforms
cmp_option(OPTION_CMP_GTC "Use GTC Codec" OFF) # Internal R&D
cmp_option(OPTION_CMP_APC "Use APC Codec" OFF) # Internal R&D

cmp_option(NO_LEGACY_BEHAVIOR "Disables any legacy behavior (before CMake migration)" ON) # this needs to be removed

# --------------------------------------------
# build system options for internal devs
# --------------------------------------------
option(OPTION_BUILD_ARCHITECTMF "Internal Dev ArchitectMF Tool" OFF)

# -----------------------------------------------------------------------------------
# Code build definitions and features: These options are been revised for v4.2
# -----------------------------------------------------------------------------------
add_compile_definitions(
        USE_ETCPACK                 # Use ETCPack for ETC2 else use CModel code!
        USE_MESH_CLI                # CLI Process Mesh (only support glTF and OBJ files)
        ENABLE_MAKE_COMPATIBLE_API  # Byte<->Float to make all source and dest compatible
        USE_OLD_SWIZZLE             # Remove swizzle flag and abide by CMP_Formats
        USE_QTWEBKIT                # Enable Qt Webengine interfaces for GUI welcome page

        # USE_CONVECTION_KERNELS                      # Test External Lib : ConvectionKernels for quality& performance, to enable use q=0.1 in HPC
        # USE_GPU_PIPELINE_VULKAN                     # Enable to use CMP_Core codecs on a Vulkan Pipeline
        # USE_CPU_PERFORMANCE_COUNTERS                # Enable to use performance monitoring with CPU high precision counters in place of GPU query performance counters
        # TEST_CMP_CORE_DECODER                       # Test CMP_Core Decoders using Compressonator SDK Decoders,
        # BC7_DEBUG_TO_RESULTS_TXT                    # Send debug info to a results text file
        # DXT5_COMPDEBUGGER                           # Remote connect data to Comp Debugger views
        # BC6H_COMPDEBUGGER                           # Remote connect data to Comp Debugger views
        # BC7_COMPDEBUGGER                            # Remote connect data to Comp Debugger views
        # BC6H_NO_OPTIMIZE_ENDPOINTS                  # Turn off BC6H optimization of endpoints - useful for debugging quantization and mode checking
        # BC6H_DEBUG_TO_RESULTS_TXT                   # Generates a Results.txt file on exe working directory; MultiThreading is turned off for debuging to text file
        # BC6H_DECODE_DEBUG                           # Enables debug info on decoder
        # GT_COMPDEBUGGER                             # Remote connect data to Comp Debugger views
        # USE_MULTIPLE_MESH_DECODE                    # Enable multiple meshes and multiple primitives draco decompression
        # USE_MESH_DRACO_SETTING                      # Expose draco settings for draco mesh compression,
        # if disabled default setting will be used for mesh compression
        #USE_GLTF2_MIPSET                             # Enable Image Transcode & Compression support for GLTF files using TextureIO
        #USE_FILEIO                                   # Used for debugging code

        #New features under devlopement
        #USE_BASIS                                    # Future release:: Universal format for transcoding codecs
        #USE_CMP_TRANSCODE                            # Future release:: Transcode BASIS/GTC to other compressed formats

        # Possible future releases features
        #ARGB_32_SUPPORT                              # Enables 32bit Float channel formats
        #SUPPORT_ETC_ALPHA                            # for ATC_RGB output enable A
        #SHOW_PROCESS_MEMORY                          # display available CPU process memory
        #USE_BCN_IMAGE_DEBUG                          # Enables Combobox in Image View for low level BCn based block compression in debug mode
        #USE_CRN                                      # Enabled .crn file output using CRUNCH encoder
        #USE_3DCONVERT                                # Enable 3D model conversion (glTF<->obj) icon
        #ENABLE_USER_ETC2S_FORMATS                    # Enable users to set these formats in CLI and GUI applications
)


if (OPTION_CMP_GTC)
    add_compile_definitions(
            USE_GTC                                      # LDR Gradient Texture Compressor patent pending
    )
endif ()

if (OPTION_CMP_APC)
    add_compile_definitions(
            USE_APC                                      # LDR Gradient Texture Compressor patent pending
    )
endif ()

# Additional options
if (OPTION_BUILD_DRACO)
    add_compile_definitions(
            USE_MESH_DRACO_EXTENSION    # Mesh Compression with Draco support in glTF and OBJ files only
            USE_KTX_EXTENSION           # Add KTX file support
    )
endif ()


if (OPTION_USE_QT)
    add_compile_definitions(CMP_USE_QT=1)
endif ()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported configuration options" FORCE)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build-out")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# '_DEBUG' is only defined by Visual Studio. Make sure it's definied everywhere
add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)


# -----------------------------------
# Set or get all package requirements
# -----------------------------------
if (COMPILER_SUPPORTS_CXX17 AND CMP_HOST_WINDOWS)
    set(CMAKE_CXX_STANDARD 17)
    add_compile_definitions(_CMP_CPP17_=1)
else ()
    # Linux
    if (COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_STANDARD 14)
        add_compile_definitions(_CMP_CPP14_=1)
    else ()
        # UNIX or LINUX
        set(CMAKE_CXX_STANDARD 11)
    endif ()
endif ()

if (CMP_HOST_WINDOWS)
    add_compile_definitions(WIN32_LEAN_AND_MEAN=1 _WIN32)

    set(CMAKE_EXE_LINKER_FLAGS "/INCREMENTAL:NO")
    # Configure Externals, they must be added via 'include' before the applications
    include(${CMAKE_ROOT}/modules/externalproject.cmake)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else ()
    if (CMP_HOST_LINUX)
        #gltf code needs _LINUX define
        add_compile_definitions(_LINUX=1)
    endif ()

    # filesystem support for c++14 
    if (COMPILER_SUPPORTS_CXX14)
        add_definitions(-std=c++14)
        link_libraries(stdc++fs)
    else ()
        if (CMP_HOST_LINUX)
            add_definitions(-std=c++11)
        endif ()
    endif ()

    add_definitions(-fPIC -Wno-write-strings)

    # ----------------------------------
    # Package requirements for the Build
    # ----------------------------------
    find_package(Threads REQUIRED)
    set(OpenGL_GL_PREFERENCE "GLVND")

    if (UNIX)
        find_package(PkgConfig REQUIRED)
    endif ()
endif ()

add_subdirectory(cmp_compressonatorlib)
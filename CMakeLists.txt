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

# Configure defines (for engine mostly)
function(wmoge_target_defs target)
    foreach (DEFINITION ${TARGET_DEFINES})
        target_compile_definitions(${target} PUBLIC ${DEFINITION})
    endforeach ()
endfunction()

# Configure defines (for engine headers mostly)
function(wmoge_target_iface_defs target)
    foreach (DEFINITION ${TARGET_DEFINES})
        target_compile_definitions(${target} INTERFACE ${DEFINITION})
    endforeach ()
endfunction()

# Plugin static library (dyn lib support and loading will be added later)
function(wmoge_plugin name)
    set(flags)
    set(args TYPE)
    set(listArgs INCLUDES SOURCES DEPENDENCIES)
    cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" ${ARGN})

    if (NOT arg_INCLUDES)
        message(FATAL_ERROR "[wmoge_game(${name})]: INCLUDES is a required argument")
    endif()
    if (NOT arg_SOURCES)
        message(FATAL_ERROR "[wmoge_game(${name})]: SOURCES is a required argument")
    endif()
    if (NOT arg_DEPENDENCIES)
        message(FATAL_ERROR "[wmoge_game(${name})]: DEPENDENCIES is a required argument")
    endif()
    if (NOT arg_TYPE)
        set(arg_TYPE STATIC) 
    endif()
    
    add_library(${name} ${arg_TYPE} ${arg_SOURCES})
    target_link_libraries(${name} PUBLIC ${arg_DEPENDENCIES})
    target_include_directories(${name} PUBLIC ${arg_INCLUDES})
    message(STATUS "Configure engine plugin '${name}' type ${arg_TYPE}")
endfunction(wmoge_plugin name sources)

# Game executable (with optional executable icon)
function(wmoge_game name)
    set(flags)
    set(args ICON)
    set(listArgs INCLUDES SOURCES DEPENDENCIES)
    cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" ${ARGN})

    if (NOT arg_INCLUDES)
        message(FATAL_ERROR "[wmoge_game(${name})]: INCLUDES is a required argument")
    endif()
    if (NOT arg_SOURCES)
        message(FATAL_ERROR "[wmoge_game(${name})]: SOURCES is a required argument")
    endif()
    if (NOT arg_DEPENDENCIES)
        message(FATAL_ERROR "[wmoge_game(${name})]: DEPENDENCIES is a required argument")
    endif()

    add_executable(${name} ${arg_SOURCES} ${arg_ICON})
    target_link_libraries(${name} PRIVATE ${arg_DEPENDENCIES})
    target_include_directories(${name} PRIVATE ${arg_INCLUDES})
    message(STATUS "Configure game executable '${name}' type APPLICATION")
endfunction()

###########################################################
# Third-party dependencies of the engine

add_subdirectory(deps)

###########################################################
# Engine sources (will be as static library)

add_subdirectory(engine)

###########################################################
# Plugins sources (standard plugins for engine)

add_subdirectory(plugins)

###########################################################
# Game template for test purposes and to kick-off new projects

add_subdirectory(template)
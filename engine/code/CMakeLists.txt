# egnine sources
file(GLOB_RECURSE ENGINE_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/**/*.hpp)

# declare static engine library
add_library(wmoge STATIC ${ENGINE_SOURCES})

# add interface to the engine for plugins
add_library(wmoge_headers INTERFACE)

# specify public include path, so user can reference engine files by modules names
target_include_directories(wmoge_headers INTERFACE ${CMAKE_CURRENT_LIST_DIR}/)

# engine (general) dependencies, which are also visible to the engine users
target_link_libraries(wmoge_headers INTERFACE cxxopts)
target_link_libraries(wmoge_headers INTERFACE tinyxml2)
target_link_libraries(wmoge_headers INTERFACE ryml)
target_link_libraries(wmoge_headers INTERFACE magic_enum)
target_link_libraries(wmoge_headers INTERFACE robin_hood)
target_link_libraries(wmoge_headers INTERFACE svector)
target_link_libraries(wmoge_headers INTERFACE yamc)

# engine (platform) dependencies, which are also visible to the engine users
target_link_libraries(wmoge_headers INTERFACE glfw)
target_link_libraries(wmoge_headers INTERFACE volk)
target_link_libraries(wmoge_headers INTERFACE vma)

# generic functions to setup target
wmoge_target_iface_defs(wmoge_headers)

# engine headers target link to engine library
target_link_libraries(wmoge PUBLIC wmoge_headers)

# engine dependencies, private
target_link_libraries(wmoge PRIVATE whereami)
target_link_libraries(wmoge PRIVATE stbimage)
target_link_libraries(wmoge PRIVATE filewatch)
target_link_libraries(wmoge PRIVATE lz4)
target_link_libraries(wmoge PRIVATE compressonator)
target_link_libraries(wmoge PRIVATE base64)
target_link_libraries(wmoge PRIVATE dynalo)
target_link_libraries(wmoge PRIVATE sha256)

# audio dependencies, private
target_link_libraries(wmoge PRIVATE OpenAL)
target_link_libraries(wmoge PRIVATE audio_file)

# lua scripting dependencies, private
target_link_libraries(wmoge PRIVATE lua_static)
target_link_libraries(wmoge PRIVATE LuaBridge)

# vulkan rendering backend related dependencies, private
target_link_libraries(wmoge PRIVATE glslang)
target_link_libraries(wmoge PRIVATE SPIRV)
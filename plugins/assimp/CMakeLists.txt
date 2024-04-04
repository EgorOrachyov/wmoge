set(PLUGIN_NAME assimp_plugin)
set(PLUGIN_DEPENDENCIES wmoge assimp)    
set(PLUGIN_SOURCES 
    assimp_plugin.cpp
    assimp_plugin.hpp
    assimp_resource_loader.cpp
    assimp_resource_loader.hpp)

wmoge_plugin(${PLUGIN_NAME} 
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ 
    SOURCES ${PLUGIN_SOURCES} 
    DEPENDENCIES ${PLUGIN_DEPENDENCIES})
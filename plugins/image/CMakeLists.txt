set(PLUGIN_NAME image_plugin)
set(PLUGIN_DEPENDENCIES wmoge)    
set(PLUGIN_SOURCES 
    image_plugin.cpp
    image_plugin.hpp
    image_asset_loader.cpp
    image_asset_loader.hpp)

wmoge_plugin(${PLUGIN_NAME} 
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ 
    SOURCES ${PLUGIN_SOURCES} 
    DEPENDENCIES ${PLUGIN_DEPENDENCIES})
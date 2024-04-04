set(PLUGIN_NAME freetype_plugin)
set(PLUGIN_DEPENDENCIES wmoge freetype)    
set(PLUGIN_SOURCES 
    freetype_font.cpp
    freetype_font.hpp
    freetype_plugin.cpp
    freetype_plugin.hpp
    freetype_resource_loader.cpp
    freetype_resource_loader.hpp)

wmoge_plugin(${PLUGIN_NAME} 
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ 
    SOURCES ${PLUGIN_SOURCES} 
    DEPENDENCIES ${PLUGIN_DEPENDENCIES})
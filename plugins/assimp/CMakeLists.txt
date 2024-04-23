set(PLUGIN_NAME assimp_plugin)
set(PLUGIN_DEPENDENCIES wmoge assimp)    
set(PLUGIN_SOURCES 
    assimp_asset_loader.cpp
    assimp_asset_loader.hpp
    assimp_import_data.hpp
    assimp_importer.cpp
    assimp_importer.hpp
    assimp_plugin.cpp
    assimp_plugin.hpp
    _rtti.hpp)

wmoge_plugin(${PLUGIN_NAME} 
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ 
    SOURCES ${PLUGIN_SOURCES} 
    DEPENDENCIES ${PLUGIN_DEPENDENCIES})
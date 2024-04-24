set(PLUGIN_NAME runtime_plugin)
set(PLUGIN_DEPENDENCIES wmoge)    
set(PLUGIN_SOURCES 
    asset/default_asset_loader.cpp
    asset/default_asset_loader.hpp
    asset/image_asset_loader.cpp
    asset/image_asset_loader.hpp
    asset/image_import_data.hpp
    asset/texture_asset_loader.cpp
    asset/texture_asset_loader.hpp
    asset/texture_import_data.hpp
    asset/wav_asset_loader.cpp
    asset/wav_asset_loader.hpp
    asset/wav_import_data.hpp
    runtime_plugin.cpp
    runtime_plugin.hpp
    _rtti.hpp)

wmoge_plugin(${PLUGIN_NAME} 
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ 
    SOURCES ${PLUGIN_SOURCES} 
    DEPENDENCIES ${PLUGIN_DEPENDENCIES})
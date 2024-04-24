set(GAME_NAME template)
set(GAME_APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/wmoge.rc")
set(GAME_DEPENDENCIES wmoge assimp_plugin freetype_plugin runtime_plugin)
set(GAME_SOURCES main.cpp)

wmoge_game(${GAME_NAME} 
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/ 
    SOURCES ${GAME_SOURCES} 
    DEPENDENCIES ${GAME_DEPENDENCIES} 
    ICON ${GAME_APP_ICON_RESOURCE})
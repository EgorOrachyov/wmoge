set(GAME_NAME template)

set(GAME_APP_ICON_RESOURCE)

if (TARGET_WINDOWS)
    set(GAME_APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/wmoge.rc")
endif ()

wmoge_game(${GAME_NAME} main.cpp ${GAME_APP_ICON_RESOURCE})
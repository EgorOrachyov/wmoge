# editor sources
file(GLOB_RECURSE EDITOR_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/**/*.hpp)

# declare static editor library
add_library(wmoge_editor STATIC ${EDITOR_SOURCES})

# add interface to the editor for plugins
add_library(wmoge_editor_headers INTERFACE)

# specify public include path, so user can reference editor files by modules names
target_include_directories(wmoge_editor_headers INTERFACE ${CMAKE_CURRENT_LIST_DIR}/)

# engine library target link to editor library
target_link_libraries(wmoge_editor PUBLIC wmoge)

# editor headers target link to editor library
target_link_libraries(wmoge_editor PUBLIC wmoge_editor_headers)
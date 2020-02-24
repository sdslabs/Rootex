cmake_minimum_required(VERSION 3.12)

# Project metadata
project(
    Rootex
    LANGUAGES CXX
    VERSION 1.0
    DESCRIPTION "Rootex Engine"
)

option(BUILD_EDITOR "Build editor executable" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_subdirectory(rootex)
add_subdirectory(game)

if (BUILD_EDITOR)
    add_subdirectory(editor)
endif(BUILD_EDITOR)

cmake_minimum_required(VERSION 3.8)

# Project metadata
project(Rootex
    LANGUAGES CXX
    VERSION 1.0
    DESCRIPTION "Rootex Engine"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setting output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "Bin")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_subdirectory(rootex)
add_subdirectory(game)

cmake_minimum_required(VERSION 3.16)

# Project metadata
project(
    Rootex
    LANGUAGES CXX
    VERSION 1.0
    DESCRIPTION "Rootex Engine"
)

set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES};ReleaseProfile;DebugProfile")
set(CMAKE_CXX_FLAGS_RELEASEPROFILE "${CMAKE_CXX_FLAGS_RELEASE} -DTRACY_ENABLE")
set(CMAKE_CXX_FLAGS_DEBUGPROFILE "${CMAKE_CXX_FLAGS_DEBUG} -DTRACY_ENABLE")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_compile_options(/bigobj)
add_compile_options(/MP)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_subdirectory(rootex)
add_subdirectory(game)
add_subdirectory(editor)

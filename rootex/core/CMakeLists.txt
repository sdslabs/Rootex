file(GLOB_RECURSE CoreSources **.cpp **.h)
file(GLOB_RECURSE CoreHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${CoreHeaders}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${CoreSources}
CACHE INTERNAL "")

add_subdirectory(renderer)

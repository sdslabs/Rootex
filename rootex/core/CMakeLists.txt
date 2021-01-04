file(GLOB_RECURSE CoreSources **.cpp **.h **.hlsl **.hlsli)
file(GLOB_RECURSE CoreHeaders **.h **.hlsl **.hlsli)

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

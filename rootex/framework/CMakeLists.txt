file(GLOB_RECURSE FrameworkSources **.cpp **.h)
file(GLOB_RECURSE FrameworkHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${FrameworkHeaders}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${FrameworkSources}
CACHE INTERNAL "")

file(GLOB_RECURSE SceneSources **.cpp **.h)
file(GLOB_RECURSE ScenetHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${SceneHeaders}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${SceneSources}
CACHE INTERNAL "")

file(GLOB_RECURSE LuaBridgeSources stub.cpp **.h)
file(GLOB_RECURSE LuaBridgeHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Source/
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${LuaBridgeSources}
CACHE INTERNAL "")

add_library(LuaBridge STATIC ${LuaBridgeSources} ${LuaBridgeHeaders})

target_include_directories(LuaBridge PUBLIC
    Source/
)
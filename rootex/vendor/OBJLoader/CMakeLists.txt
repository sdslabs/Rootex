file(GLOB_RECURSE OBJLoaderSources stub.cpp **.h)
file(GLOB_RECURSE OBJLoaderHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Source/
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${OBJLoaderSources}
CACHE INTERNAL "")

add_library(OBJLoader STATIC ${OBJLoaderSources} ${OBJLoaderHeaders})

set_target_properties(OBJLoader PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(OBJLoader PUBLIC
    Source/
)
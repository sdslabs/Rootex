file(GLOB_RECURSE JSONSources stub.cpp **.hpp)
file(GLOB_RECURSE JSONHeaders **.hpp)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Source/
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${JSONSources}
CACHE INTERNAL "")

add_library(JSON STATIC ${JSONSources} ${JSONHeaders})

target_include_directories(JSON PUBLIC
    ./
)
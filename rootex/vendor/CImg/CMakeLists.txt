file(GLOB_RECURSE CImgSources stub.cpp **.hpp)
file(GLOB_RECURSE CImgHeaders **.hpp)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Source/
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${CImgSources}
CACHE INTERNAL "")

add_library(CImg STATIC ${CImgSources} ${CImgHeaders})
set_property(TARGET CImg PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(CImg PUBLIC
    ./
)
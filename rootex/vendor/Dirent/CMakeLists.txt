file(GLOB_RECURSE DirentSources stub.cpp **.cpp)
file(GLOB_RECURSE DirentHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${DirentSources}
CACHE INTERNAL "")

add_library(Dirent STATIC ${DirentSources} ${DirentHeaders})
set_property(TARGET Dirent PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(Dirent PUBLIC
    ./include/
)

file(GLOB_RECURSE MediaFoundation ./**.cpp)
file(GLOB_RECURSE MediaFoundationH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/
CACHE INTERNAL "")

add_library(MediaFoundation STATIC ${MediaFoundation} ${MediaFoundationH})
set_property(TARGET MediaFoundation PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_target_properties(MediaFoundation PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(MediaFoundation PUBLIC
    ./
)

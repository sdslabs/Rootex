file(GLOB_RECURSE DirectXTK ./**.cpp)
file(GLOB_RECURSE DirectXTKH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Inc/
CACHE INTERNAL "")

add_library(DirectXTK STATIC ${DirectXTK} ${DirectXTKH})
set_property(TARGET DirectXTK PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_target_properties(DirectXTK PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(DirectXTK PUBLIC
    ./Inc/
)

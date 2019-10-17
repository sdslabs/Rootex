file(GLOB_RECURSE CommonSources *.cpp *.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ./rootex
CACHE INTERNAL "")

add_library(Rootex STATIC ${CommonSources})

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(os)
add_subdirectory(vendor)
add_subdirectory(script)

target_include_directories(Rootex PUBLIC
    ../${ROOTEX_INCLUDES}
)

target_link_libraries(Rootex PUBLIC
    RootexCommon
    RootexCore
    RootexOS
    RootexScript
    RootexVendor
    Lua
	d3d11.lib
)

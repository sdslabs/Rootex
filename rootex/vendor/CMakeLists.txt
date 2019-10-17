file(GLOB_RECURSE VendorSources stub.cpp)

add_subdirectory(Lua)
add_subdirectory(PortAudio)

add_library(RootexVendor STATIC ${VendorSources})

target_include_directories(RootexVendor PUBLIC ../)

target_link_libraries(RootexVendor PUBLIC
    Lua
    # LuaBridge
	PortAudio
)

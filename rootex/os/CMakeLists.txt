file(GLOB_RECURSE OSSources **.cpp **.h)

add_library(RootexOS STATIC ${OSSources})

target_include_directories(RootexOS PUBLIC ../)

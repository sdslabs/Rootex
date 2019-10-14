file(GLOB_RECURSE CommonSources **.cpp **.h)

add_library(RootexCommon STATIC ${CommonSources})

target_include_directories(RootexCommon PUBLIC ../)

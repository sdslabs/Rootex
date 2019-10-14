file(GLOB_RECURSE CoreSources **.cpp **.h)

add_library(RootexCore STATIC ${CoreSources})

target_include_directories(RootexCore PUBLIC ../)

file(GLOB_RECURSE LuaBridge Source/**.h)

add_library(LuaBridge STATIC ${LuaBridge})
target_include_directories(RootexCore PUBLIC Source/)

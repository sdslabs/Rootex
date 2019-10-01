file(GLOB_RECURSE LuaCAPI src/**.c)
file(GLOB_RECURSE LuaCAPIH src/**.h)

add_library(Lua STATIC ${LuaCAPI} ${LuaCAPIH})
target_include_directories(RootexCore PUBLIC src/)

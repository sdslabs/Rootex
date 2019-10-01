file(GLOB_RECURSE LuaCAPI src/**.c)
file(GLOB_RECURSE LuaCAPIH src/**.h)

add_library(Lua STATIC ${LuaCAPI} ${LuaCAPIH})
set_target_properties(Lua PROPERTIES LINKER_LANGUAGE CXX)

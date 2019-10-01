file(GLOB_RECURSE LuaBridge Source/**.h)

add_library(LuaBridge STATIC ${LuaBridge})
set_target_properties(LuaBridge PROPERTIES LINKER_LANGUAGE CXX)

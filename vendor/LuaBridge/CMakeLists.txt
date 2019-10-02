file(GLOB_RECURSE LuaBridge
	Source/LuaBridge/*.h
	Source/LuaBridge/detail/*.h
)

add_library(LuaBridge STATIC ${LuaBridge})
set_target_properties(LuaBridge PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(LuaBridge PUBLIC
	../Lua/src/
	Source/
	Source/LuaBridge/
	Source/LuaBridge/detail
)

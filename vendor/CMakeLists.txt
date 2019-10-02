add_subdirectory(Lua)
add_subdirectory(LuaBridge)

add_library(Vendor STATIC "" Lua LuaBridge)
set_target_properties(Vendor PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Vendor PUBLIC
	..
	Lua/src/
	LuaBridge/Source/
	LuaBridge/Source/LuaBridge
)

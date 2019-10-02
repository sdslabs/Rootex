add_subdirectory(Lua)
add_subdirectory(LuaBridge)

add_library(Vendor STATIC "" Lua LuaBridge)

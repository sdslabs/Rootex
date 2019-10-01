add_subdirectory(Lua)
add_subdirectory(LuaBridge)

target_link_libraries(RootexCore PUBLIC Lua LuaBridge)

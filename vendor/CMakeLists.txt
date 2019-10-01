add_subdirectory(Lua)
add_subdirectory(LuaBridge)

add_library(Vendor STATIC stub.cpp)
set_target_properties(Vendor PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(Vendor PUBLIC Lua LuaBridge)

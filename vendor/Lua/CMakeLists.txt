file(GLOB_RECURSE LuaCAPI ./**.c)
file(GLOB_RECURSE LuaCAPIH ./**.h)

set_source_files_properties(${LuaCAPI} PROPERTIES LANGUAGE CXX)

add_library(Lua STATIC ${LuaCAPI})
set_target_properties(Lua PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Lua)
include_directories(src/)

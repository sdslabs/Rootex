file(GLOB_RECURSE LuaCAPI ./**.c)
file(GLOB_RECURSE LuaCAPIH ./**.h)

set_source_files_properties(${LuaCAPI} ${LuaCAPIH} PROPERTIES LANGUAGE CXX)

add_library(Lua STATIC ${LuaCAPI} ${LUACAPIH})
set_target_properties(Lua PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Lua PUBLIC
    src/
)

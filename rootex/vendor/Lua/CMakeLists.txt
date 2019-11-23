file(GLOB_RECURSE LuaCAPI ./**.c)
file(GLOB_RECURSE LuaCAPIH ./**.h)

set_source_files_properties(${LuaCAPI} ${LuaCAPIH} PROPERTIES LANGUAGE C)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/src/
CACHE INTERNAL "")

add_library(Lua STATIC ${LuaCAPI} ${LUACAPIH})
set_target_properties(Lua PROPERTIES LINKER_LANGUAGE C)

target_include_directories(Lua PUBLIC
    src/
)

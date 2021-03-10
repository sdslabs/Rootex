file(GLOB_RECURSE LPeg ./**.c)
file(GLOB_RECURSE LPegH ./**.h)

set_source_files_properties(${LPeg} ${LPegH} PROPERTIES LANGUAGE C)

set(ROOTEX_INCLUDES
	${ROOTEX_INCLUDES}
	${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_library(LPeg STATIC ${LPeg} ${LPegH})
set_target_properties(LPeg PROPERTIES LINKER_LANGUAGE C)
set_property(TARGET LPeg PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(LPeg PUBLIC
	../Lua/src/
)

target_link_libraries(LPeg PUBLIC
	Lua
)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ""
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_ROOTEX_SOURCES}
    ""
CACHE INTERNAL "")

add_subdirectory(common EXCLUDE_FROM_ALL)
add_subdirectory(core EXCLUDE_FROM_ALL)
add_subdirectory(os EXCLUDE_FROM_ALL)
add_subdirectory(vendor EXCLUDE_FROM_ALL)
add_subdirectory(script EXCLUDE_FROM_ALL)

add_library(Rootex STATIC ${ROOTEX_SOURCES})

target_include_directories(Rootex PUBLIC
    ${ROOTEX_INCLUDES}
)

target_link_libraries(Rootex PUBLIC
    Lua
    LuaBridge
    OpenAL
	d3d11.lib
)

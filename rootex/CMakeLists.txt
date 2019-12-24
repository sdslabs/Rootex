set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ""
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ""
CACHE INTERNAL "")

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(framework)
add_subdirectory(os)
add_subdirectory(scene)
add_subdirectory(script)
add_subdirectory(vendor)

add_library(Rootex STATIC ${ROOTEX_SOURCES})

source_group(TREE "../"
    PREFIX "Rootex"
    FILES ${ROOTEX_SOURCES}
)

target_include_directories(Rootex PUBLIC
    ${ROOTEX_INCLUDES}
)

target_link_libraries(Rootex PUBLIC
    Lua
    LuaBridge
    OpenAL
    ALUT
    SimpleMath
    d3d11.lib
    D3DCompiler.lib
)

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

set(ROOTEX_PCH_HEADERS
CACHE INTERNAL "")

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(framework)
add_subdirectory(main)
add_subdirectory(os)
add_subdirectory(script)
add_subdirectory(vendor)
add_subdirectory(pch)

# Adding shaders for pre-build compilation
file(GLOB ShaderFiles
    ./core/renderer/shaders/*.hlsl
)

file(GLOB VertexShaderFiles
    ./core/renderer/shaders/*vertex_shader.hlsl
)

file(GLOB PixelShaderFiles
    ./core/renderer/shaders/*pixel_shader.hlsl
)

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${ShaderFiles}
CACHE INTERNAL "")

string(CONCAT ShaderCompileDestination ${CMAKE_CURRENT_LIST_DIR} "/core/renderer/shaders/%\(Filename\).cso")

set_source_files_properties(${ShaderFiles} 
    PROPERTIES
        VS_SHADER_MODEL 4.0
        VS_SHADER_OBJECT_FILE_NAME ${ShaderCompileDestination}
        VS_SHADER_ENABLE_DEBUG true
        VS_SHADER_DISABLE_OPTIMIZATIONS true
)

set_source_files_properties(${VertexShaderFiles}
    PROPERTIES 
        VS_SHADER_TYPE Vertex
)

set_source_files_properties(${PixelShaderFiles}
    PROPERTIES 
        VS_SHADER_TYPE Pixel
)

if (BUILD_EDITOR)
    add_definitions(-DROOTEX_EDITOR)
endif(BUILD_EDITOR)

add_library(Rootex STATIC ${ROOTEX_SOURCES})

target_precompile_headers(Rootex PUBLIC ${ROOTEX_PCH_HEADERS})

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
    LuaBindings
    OpenAL
    ALUT
    DirectXTK
    ImGUI
    Gainput
    d3d11.lib
    xinput.lib
    D3DCompiler.lib
)

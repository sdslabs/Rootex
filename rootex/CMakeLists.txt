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

file(GLOB PCHHeader common/types.h)

add_subdirectory(common)
add_subdirectory(app)
add_subdirectory(core)
add_subdirectory(framework)
add_subdirectory(main)
add_subdirectory(os)
add_subdirectory(script)

add_definitions(-DRMLUI_STATIC_LIB)
add_subdirectory(vendor)

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

string(CONCAT ShaderCompileDestination ${CMAKE_CURRENT_LIST_DIR} "/assets/shaders/%\(Filename\).cso")

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

target_precompile_headers(Rootex PUBLIC ${PCHHeader})

source_group(TREE "../"
    PREFIX "Rootex"
    FILES ${ROOTEX_SOURCES}
)

target_include_directories(Rootex PUBLIC
    ${ROOTEX_INCLUDES}
)

target_link_libraries(Rootex PUBLIC
    Lua
    Sol3
    OpenAL
    ALUT
    DirectXTK
    JSON
    ImGui
    ImGuizmo
    Gainput
    Bullet3D
    assimp
    RMLUI
    Shell32.lib
    d3d11.lib
    xinput.lib
    D3DCompiler.lib
)

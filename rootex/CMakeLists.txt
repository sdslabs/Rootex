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
add_subdirectory(main)
add_subdirectory(os)
add_subdirectory(scene)
add_subdirectory(script)
add_subdirectory(vendor)

# Adding shaders for pre-build compilation
file(GLOB ShaderFiles
./core/renderer/shaders/*.hlsl
)

file(GLOB VertexShaderFiles
./core/renderer/shaders/VertexShader.hlsl
)

file(GLOB PixelShaderFiles
./core/renderer/shaders/PixelShader.hlsl
)

set(ROOTEX_SOURCES
${ROOTEX_SOURCES}
${ShaderFiles}
CACHE INTERNAL "")

string(CONCAT ShaderCompileDestination ${PROJECT_BINARY_DIR} "/Game/%\(Filename\).cso")

set_source_files_properties(${ShaderFiles} 
    PROPERTIES
        VS_SHADER_MODEL 4.0
        VS_SHADER_OBJECT_FILE_NAME ${ShaderCompileDestination}
        VS_SHADER_ENABLE_DEBUG true
)

set_source_files_properties(${VertexShaderFiles}
    PROPERTIES 
        VS_SHADER_TYPE Vertex
)

set_source_files_properties(${PixelShaderFiles}
    PROPERTIES 
        VS_SHADER_TYPE Pixel
)

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

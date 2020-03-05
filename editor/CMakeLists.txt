file(GLOB_RECURSE EditorSource ./**.cpp)
file(GLOB_RECURSE EditorHeaders ./**.h)
file(GLOB_RECURSE EditorLuaScripts ./**.lua)

set_source_files_properties(${EditorLuaScripts} PROPERTIES
    HEADER_FILE_ONLY TRUE
)

add_definitions(-DROOTEX_EDITOR)
add_executable(Editor ${EditorSource} ${EditorHeaders} ${EditorLuaScripts})

target_include_directories(Editor PUBLIC ../)
target_link_libraries(Editor PUBLIC Rootex)

source_group(TREE "../editor/"
    PREFIX "Editor"
    FILES ${EditorSource} ${EditorHeaders} ${EditorLuaScripts}
)

add_custom_command(TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ALUT_DLL_LIBRARY}
        $<TARGET_FILE_DIR:Editor>)
       
set_directory_properties(PROPERTIES 
    VS_STARTUP_PROJECT Editor
)

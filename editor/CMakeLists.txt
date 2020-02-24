file(GLOB_RECURSE EditorSource ./**.cpp)
file(GLOB_RECURSE EditorHeaders ./**.h)

add_executable(Editor ${EditorSource} ${EditorHeaders})

target_include_directories(Editor PUBLIC ../)
target_link_libraries(Editor PUBLIC Rootex)

add_custom_command(TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ALUT_DLL_LIBRARY}
        $<TARGET_FILE_DIR:Editor>)
       
set_directory_properties(PROPERTIES 
    VS_STARTUP_PROJECT Editor
)

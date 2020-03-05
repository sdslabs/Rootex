file(GLOB_RECURSE GameSource ./**.cpp)
file(GLOB_RECURSE GameHeaders ./**.h)
file(GLOB_RECURSE GameLuaScripts ./**.lua)

set_source_files_properties(${GameLuaScripts} PROPERTIES
    HEADER_FILE_ONLY TRUE
)

add_executable(Game ${GameSource} ${GameHeaders} ${GameLuaScripts})

target_include_directories(Game PUBLIC ../)
target_link_libraries(Game PUBLIC Rootex)

source_group(TREE "../game/"
    PREFIX "Game"
    FILES ${GameSource} ${GameHeaders} ${GameLuaScripts}
)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ALUT_DLL_LIBRARY}
        $<TARGET_FILE_DIR:Game>)
       
set_directory_properties(PROPERTIES 
    VS_STARTUP_PROJECT Game
)

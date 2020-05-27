file(GLOB_RECURSE GameSource ./**.cpp)
file(GLOB_RECURSE GameHeaders ./**.h)
file(GLOB_RECURSE GameLuaScripts ./**.lua)
file(GLOB_RECURSE GameJSONs ./**.json)

set_source_files_properties(${GameLuaScripts} ${GameJSONs} PROPERTIES
    HEADER_FILE_ONLY TRUE
)

add_executable(Game ${GameSource} ${GameHeaders} ${GameLuaScripts} ${GameJSONs})

target_include_directories(Game PUBLIC ../)
target_link_libraries(Game PUBLIC Rootex)
add_dependencies(Game Rootex)

source_group(TREE "../game/"
    PREFIX "Game"
    FILES ${GameSource} ${GameHeaders} ${GameLuaScripts} ${GameJSONs}
)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ALUT_DLL_LIBRARY}
        ${RMLUI_FREETYPE_DLL_LIBRARY}
        $<TARGET_FILE_DIR:Game>)
       
set_directory_properties(PROPERTIES 
    VS_STARTUP_PROJECT Game
)

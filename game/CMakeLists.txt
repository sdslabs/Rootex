file(GLOB_RECURSE GameSource ./**.cpp)
file(GLOB_RECURSE GameHeaders ./**.h)

add_executable(Game ${GameSource} ${GameHeaders})

target_include_directories(Game PUBLIC ../)
target_link_libraries(Game PUBLIC Rootex)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ALUT_DLL_LIBRARY}
        $<TARGET_FILE_DIR:Game>)
       
set_directory_properties(PROPERTIES 
    VS_STARTUP_PROJECT Game
)

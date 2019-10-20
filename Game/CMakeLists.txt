file(GLOB_RECURSE GameSource ./**.cpp)
file(GLOB_RECURSE GameHeaders ./**.h)

add_executable(Game ${GameSource} ${GameHeaders})

target_include_directories(Game PUBLIC ../)
target_link_libraries(Game PUBLIC Rootex)

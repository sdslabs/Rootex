file(GLOB_RECURSE GameSource ./**.cpp)
file(GLOB_RECURSE GameHeaders ./**.h)

add_executable(Game ${GameSource} ${GameHeaders})
target_link_libraries(Game PUBLIC RootexCore)
target_include_directories(Game PRIVATE ./ ../Source)

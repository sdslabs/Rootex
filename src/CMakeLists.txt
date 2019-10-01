add_subdirectory(core)
add_subdirectory(os)
add_subdirectory(script)

file(GLOB_RECURSE Commons *.cpp *.h)

add_library(Source STATIC ${Commons})

target_link_libraries(RootexCore PUBLIC Source Core OS Script)

file(GLOB_RECURSE OSSource **.cpp)
file(GLOB_RECURSE OSHeaders **.h)

add_library(OS STATIC ${OSSource} ${OSHeaders})

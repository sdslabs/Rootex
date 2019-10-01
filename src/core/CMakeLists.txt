file(GLOB_RECURSE CoreSource **.cpp)
file(GLOB_RECURSE CoreHeaders **.h)

add_library(Core STATIC ${CoreSource} ${CoreHeaders})

file(GLOB_RECURSE ScriptSource **.cpp)
file(GLOB_RECURSE ScriptHeaders **.h)

add_library(Script STATIC ${ScriptSource} ${ScriptHeaders})

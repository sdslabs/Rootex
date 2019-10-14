file(GLOB_RECURSE ScriptSources **.cpp **.h)

add_library(RootexScript STATIC ${ScriptSources})

target_include_directories(RootexScript PUBLIC
    ../
    ../vendor/LuaBridge/Source/
)

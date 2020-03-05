set(CMAKE_SWIG_FLAGS "")
set_source_files_properties(rootex.i PROPERTIES CPLUSPLUS ON)

swig_add_library(
    LuaBindings
    TYPE STATIC
    LANGUAGE lua
    SOURCES rootex.i
)

set_property(TARGET LuaBindings PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES true)
swig_link_libraries(LuaBindings Rootex)

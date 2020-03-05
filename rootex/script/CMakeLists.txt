file(GLOB_RECURSE ScriptSources **.cpp **.h)
file(GLOB_RECURSE ScriptHeaders **.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${ScriptHeaders}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${ScriptSources}
CACHE INTERNAL "")

if(BUILD_LUA)
    cmake_policy(SET CMP0078 NEW)
    cmake_policy(SET CMP0086 NEW)
    set(SWIG_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/../vendor/SWIG/swig.exe")    

    find_package(SWIG REQUIRED)
    
    include(${SWIG_USE_FILE})
    
    set(CMAKE_SWIG_FLAGS "")

    set(ROOTEX_SOURCES
        ${ROOTEX_SOURCES}
        ${ROOTEX_SWIG_INTERFACE}
    CACHE INTERNAL "")
    set_source_files_properties(${ROOTEX_SWIG_INTERFACE} PROPERTIES CPLUSPLUS ON)
    swig_add_library(
        LuaBindings
        TYPE STATIC
        LANGUAGE lua
        SOURCES ${ROOTEX_SWIG_INTERFACE}
    )
    set_target_properties(LuaBindings PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(LuaBindings PUBLIC 
        ${ROOTEX_INCLUDES}
        ${CMAKE_CURRENT_LIST_DIR}/../vendor/Lua/src/
    )

endif()

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
    cmake_policy(SET CMP0078 OLD)
    cmake_policy(SET CMP0086 OLD)
    set(SWIG_EXECUTABLE "D:\\CPP Projects\\Rootex\\rootex\\vendor\\SWIG\\swig.exe")    
    find_package(SWIG REQUIRED)
    
    include(${SWIG_USE_FILE})
    
    add_subdirectory(interface)
endif()

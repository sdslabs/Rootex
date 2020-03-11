cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)

set(SWIG_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/swig.exe")    
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${ROOTEX_SWIG_INTERFACE}
CACHE INTERNAL "")

set_source_files_properties(${ROOTEX_SWIG_INTERFACE} PROPERTIES CPLUSPLUS ON)
swig_add_library(
    SWIGBindings
    TYPE STATIC
    LANGUAGE lua
    SOURCES ${ROOTEX_SWIG_INTERFACE}
)
set_property(TARGET SWIGBindings PROPERTY SWIG_INCLUDE_DIRECTORIES
    ${ROOTEX_INCLUDES}
)
set_target_properties(SWIGBindings PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(SWIGBindings PUBLIC 
    ${ROOTEX_INCLUDES}
    ../Lua/src/
)

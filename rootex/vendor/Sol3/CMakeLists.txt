file(GLOB_RECURSE Sol3Sources stub.cpp **.hpp)
file(GLOB_RECURSE Sol3Headers **.hpp)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${Sol3Sources}
CACHE INTERNAL "")

add_library(Sol3 STATIC ${Sol3Sources} ${Sol3Headers})
set_property(TARGET Sol3 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(Sol3 PUBLIC
    ./include
)
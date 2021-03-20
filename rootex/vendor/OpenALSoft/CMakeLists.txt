file(GLOB_RECURSE OpenALSoftH ./**.h)

set(OPENALSOFT_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/Win64/OpenAL32.lib")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_library(OpenALSoft STATIC ${OpenalSoftH} stub.cpp)
set_property(TARGET OpenALSoft PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(OpenALSoft PRIVATE
    ./include/
)

target_link_libraries(OpenALSoft PRIVATE
    ${OPENALSOFT_LIBRARY}
)

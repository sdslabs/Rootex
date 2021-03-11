file(GLOB_RECURSE FreeType ./**.cpp)
file(GLOB_RECURSE FreeTypeH ./**.h)

set(FREETYPE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/freetype.lib")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_library(FreeType STATIC ${FreeType} ${FreeTypeH})
set_property(TARGET FreeType PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(FreeType PRIVATE
    ./include/
)

target_link_libraries(FreeType PRIVATE
    ${FREETYPE_LIBRARY}
)

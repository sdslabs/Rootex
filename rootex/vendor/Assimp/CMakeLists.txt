set(ZLIB_LIBRARIES zlibstatic)

add_subdirectory(assimp)
set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/assimp/include/
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/
CACHE INTERNAL "")

# TODO - Looking at the cmake of assimp it seems that we'll need to do more configuration stuff efore Release. example- check INJECT_DEBUG_POSTFIX in that cmake
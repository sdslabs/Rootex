set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(assimp)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/assimp/include/
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/
CACHE INTERNAL "")

# TODO - Looking at the cmake of assimp it seems that we'll need to do more configuration stuff efore Release. example- check INJECT_DEBUG_POSTFIX in that cmake
set(BULLET3D_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/src/")
set(BULLET3D_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/Win64/")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/src/
CACHE INTERNAL "")

add_library(Bullet3D STATIC stub.cpp)

target_include_directories(Bullet3D PUBLIC
    ${BULLET3D_INCLUDE_DIR}
)

target_link_libraries(Bullet3D PUBLIC
    ${BULLET3D_LIBRARY}/BulletCollision_vs2010_x64_debug.lib
    ${BULLET3D_LIBRARY}/BulletDynamics_vs2010_x64_debug.lib
    ${BULLET3D_LIBRARY}/LinearMath_vs2010_x64_debug.lib
)
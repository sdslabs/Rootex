file(GLOB_RECURSE Bullet3D ./**.cpp)
file(GLOB_RECURSE Bullet3DH ./**.h)

set(BULLET3D_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/src/
CACHE INTERNAL "")

add_definitions(-DB3_USE_CLEW)
add_definitions(-DBT_USE_SSE_IN_API)
add_definitions(-DBT_USE_SSE)
add_library(Bullet3D STATIC ${Bullet3D} ${Bullet3DH})

target_include_directories(Bullet3D PUBLIC
    ${BULLET3D_INCLUDE_DIR}
)

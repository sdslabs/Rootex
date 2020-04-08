file(GLOB_RECURSE VendorSources stub.cpp)
file(GLOB_RECURSE VendorHeaders *.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${VendorHeaders}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${VendorSources}
CACHE INTERNAL "")

add_subdirectory(Lua)
add_subdirectory(Sol3)
add_subdirectory(Bullet3D)
add_subdirectory(OpenAL)
add_subdirectory(DirectXTK)
add_subdirectory(Gainput)
add_subdirectory(OBJLoader)
add_subdirectory(ImGUI)
add_subdirectory(JSON)
add_subdirectory(Assimp)
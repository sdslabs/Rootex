set(GLEW_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/")
set(GLEW_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/glew32sd.lib")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_library(GLEW SHARED stub.cpp)

target_include_directories(GLEW PUBLIC
    ${GLEW_INCLUDE_DIR}
)

target_link_libraries(GLEW PUBLIC
    ${GLEW_LIBRARY}
    opengl32.lib
)

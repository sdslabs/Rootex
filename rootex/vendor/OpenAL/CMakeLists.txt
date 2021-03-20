# Not using find_package for OpenAL because the modules won't find it
# find_package(OpenAL)
set(OPENAL_FOUND ON PARENT_SCOPE)
set(OPENAL_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/" PARENT_SCOPE)
set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_library(OpenAL SHARED stub.cpp)
set_property(TARGET OpenAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(OpenAL PUBLIC
    ${OPENAL_INCLUDE_DIR}
)

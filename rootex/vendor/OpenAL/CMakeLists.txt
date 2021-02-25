# Not using find_package for OpenAL because the modules won't find it
# find_package(OpenAL)
set(OPENAL_FOUND ON PARENT_SCOPE)
set(OPENAL_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/" PARENT_SCOPE)
set(OPENAL_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/Win64/OpenAL32.lib" PARENT_SCOPE)
set(OPENAL_DLL_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/Win64/OpenAL32.dll" CACHE INTERNAL "")
set(WRAP_OAL_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/libs/Win64/wrap_oal.dll" CACHE INTERNAL "")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_library(OpenAL SHARED stub.cpp)

target_include_directories(OpenAL PUBLIC
    ${OPENAL_INCLUDE_DIR}
)

target_link_libraries(OpenAL PUBLIC
    ${OPENAL_LIBRARY}
)

set(OPENAL_INCLUDE_DIR "C:/Program Files (x86)/OpenAL 1.1 SDK/include")
set(OPENAL_LIBRARY "C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64/OpenAL32.lib")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_library(OpenAL SHARED stub.cpp)
set_property(TARGET OpenAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(OpenAL PUBLIC
    ${OPENAL_INCLUDE_DIR}
)

target_link_libraries(OpenAL PUBLIC
    ${OPENAL_LIBRARY}
)

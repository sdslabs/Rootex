file(GLOB_RECURSE Rlottie rlottie/src/binding/**.cpp rlottie/src/lottie/**.cpp rlottie/src/vector/**.cpp)
file(GLOB_RECURSE RlottieH rlottie/inc/**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/rlottie/inc/
CACHE INTERNAL "")

add_library(Rlottie STATIC ${Rlottie} ${RlottieH})
set_property(TARGET Rlottie PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(Rlottie PUBLIC
    rlottie/inc/
    rlottie/src/vector/
    rlottie/src/vector/freetype
    rlottie/vs2019/
)

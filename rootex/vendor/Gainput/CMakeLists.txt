file(GLOB_RECURSE Gainput ./**.cpp)
file(GLOB_RECURSE GainputH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include/
CACHE INTERNAL "")

add_definitions(-DGAINPUT_BUILD_SHARED=OFF)
add_definitions(-DGAINPUT_BUILD_STATIC=ON)
add_definitions(-DGAINPUT_LIB_BUILD)
add_definitions(-DGAINPUT_PLATFORM_WIN)

add_library(Gainput STATIC ${Gainput} ${GainputH})
set_target_properties(Gainput PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Gainput PUBLIC
    ./include/
)

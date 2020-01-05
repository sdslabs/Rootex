file(GLOB_RECURSE SimpleMath ./**.cpp)
file(GLOB_RECURSE SimpleMathH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_library(SimpleMath STATIC ${SimpleMath} ${SimpleMathH})
set_target_properties(SimpleMath PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(SimpleMath PUBLIC
    ./
)

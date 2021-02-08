file(GLOB_RECURSE ASSAO ./**.cpp)
file(GLOB_RECURSE ASSAOH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/
CACHE INTERNAL "")

add_library(ASSAO STATIC ${ASSAO} ${ASSAOH})
set_target_properties(ASSAO PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ASSAO PUBLIC
    ./
)

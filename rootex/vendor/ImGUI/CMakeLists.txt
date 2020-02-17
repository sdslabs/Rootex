file(GLOB_RECURSE ImGUI ./**.cpp)
file(GLOB_RECURSE ImGUIH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_library(ImGUI STATIC ${ImGUI} ${ImGUIH})
set_target_properties(ImGUI PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ImGUI PUBLIC
    ./
)

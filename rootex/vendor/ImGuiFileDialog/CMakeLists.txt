file(GLOB_RECURSE ImGuiFileDialog ./**.cpp)
file(GLOB_RECURSE ImGuiFileDialogH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_library(ImGuiFileDialog STATIC ${ImGuiFileDialog} ${ImGuiFileDialogH})
set_target_properties(ImGuiFileDialog PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ImGuiFileDialog PUBLIC
    ./
    ../ImGui/
    ../Dirent/include/
    ../../../
)

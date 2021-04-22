file(GLOB_RECURSE ImGuiColorTextEdit ./**.cpp)
file(GLOB_RECURSE ImGuiColorTextEditH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_library(ImGuiColorTextEdit STATIC ${ImGuiColorTextEdit} ${ImGuiColorTextEditH})
set_property(TARGET ImGuiColorTextEdit PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_target_properties(ImGuiColorTextEdit PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ImGuiColorTextEdit PUBLIC
    ./
    ../ImGui/
)

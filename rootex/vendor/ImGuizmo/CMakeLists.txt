file(GLOB_RECURSE ImGuizmo ./**.cpp)
file(GLOB_RECURSE ImGuizmoH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

add_library(ImGuizmo STATIC ${ImGuizmo} ${ImGuizmoH})
set_target_properties(ImGuizmo PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(ImGuizmo PUBLIC
    ./
    ../ImGui/
)

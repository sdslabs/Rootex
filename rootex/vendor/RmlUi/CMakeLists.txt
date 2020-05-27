set(RMLUI_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/Include/")

set(RMLUI_CONTROLS_RELEASE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Bin/Static-Release/RmlControls.lib")
set(RMLUI_CORE_RELEASE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Bin/Static-Release/RmlCore.lib")
set(RMLUI_DEBUGGER_RELEASE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Bin/Static-Release/RmlDebugger.lib")

set(RMLUI_CONTROLS_DEBUG_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Bin/Static-Debug/RmlControls.lib")
set(RMLUI_CORE_DEBUG_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Bin/Static-Debug/RmlCore.lib")
set(RMLUI_DEBUGGER_DEBUG_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Bin/Static-Debug/RmlDebugger.lib")

set(RMLUI_FREETYPE_DLL_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Dependencies/freetype-2.10.1/freetype.dll" CACHE INTERNAL "")
set(RMLUI_FREETYPE_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Dependencies/freetype-2.10.1/freetype.lib")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Include/
CACHE INTERNAL "")

add_library(RMLUI STATIC stub.cpp)

target_include_directories(RMLUI PUBLIC
    ${RMLUI_INCLUDE_DIR}
    ./Dependencies
)

target_link_libraries(RMLUI PUBLIC
    debug ${RMLUI_CONTROLS_DEBUG_LIBRARY}
    optimized ${RMLUI_CONTROLS_RELEASE_LIBRARY}
)

target_link_libraries(RMLUI PUBLIC
    ${RMLUI_FREETYPE_LIBRARY}
)

target_link_libraries(RMLUI PUBLIC
    debug ${RMLUI_CORE_DEBUG_LIBRARY}
    optimized ${RMLUI_CORE_RELEASE_LIBRARY}
)

target_link_libraries(RMLUI PUBLIC
    debug ${RMLUI_DEBUGGER_DEBUG_LIBRARY}
    optimized ${RMLUI_DEBUGGER_RELEASE_LIBRARY}
)

file(GLOB_RECURSE ScriptSources **.cpp **.h)
file(GLOB_RECURSE ScriptHeaders **.h)
file(GLOB_RECURSE ScriptInterfaceFiles ./interface/**.i)

set(ROOTEX_SWIG_INTERFACE
    ${ROOTEX_SWIG_INTERFACE}
    ${ScriptInterfaceFiles}
CACHE INTERNAL "")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${ScriptHeaders}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${ScriptSources}
CACHE INTERNAL "")

file(GLOB_RECURSE Effekseer 
	effekseer/src/**Effekseer**.cpp
	effekseer/src/**Culling**.cpp
)
file(GLOB_RECURSE EffekseerH ./**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/effekseer/src/
    ${CMAKE_CURRENT_LIST_DIR}/effekseer/src/include/
CACHE INTERNAL "")

add_definitions(-D__EFFEKSEER_RENDERER_INTERNAL_LOADER__)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG_EFFEKSEER")

add_library(Effekseer STATIC ${Effekseer} ${EffekseerH})
set_property(TARGET Effekseer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_target_properties(Effekseer PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Effekseer PUBLIC
	effekseer/src/
    effekseer/src/Effekseer/
    effekseer/src/include/
	effekseer/src/EffekseerSoundAL/AL/include
)

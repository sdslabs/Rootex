file(GLOB_RECURSE RmlCore Source/Core/**.cpp)
file(GLOB_RECURSE RmlCoreH Source/Core/**.h Source/Core/**.hpp)

file(GLOB_RECURSE RmlDebugger Source/Debugger/**.cpp)
file(GLOB_RECURSE RmlDebuggerH Source/Debugger/**.h Source/Debugger/**.hpp)

file(GLOB_RECURSE RmlLua Source/Lua/**.cpp)
file(GLOB_RECURSE RmlLuaH Source/Lua/**.h Source/Lua/**.hpp)

file(GLOB_RECURSE RmlLottie Samples/basic/lottie/src/ElementLottie.cpp Samples/basic/lottie/src/LottiePlugin.cpp)
file(GLOB_RECURSE RmlLottieH Samples/basic/lottie/src/ElementLottie.h Samples/basic/lottie/src/LottiePlugin.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/Include/
    ${CMAKE_CURRENT_LIST_DIR}/Samples/basic/lottie/src/
CACHE INTERNAL "")

add_library(RmlCore STATIC ${RmlCore} ${RmlCoreH})
add_library(RmlDebugger STATIC ${RmlDebugger} ${RmlDebuggerH})
add_library(RmlLua STATIC ${RmlLua} ${RmlLua})
add_library(RmlLottie STATIC ${RmlLottie} ${RmlLottieH})

target_include_directories(RmlCore PUBLIC
	Include/
	Source/
	../Freetype/include/
)

target_include_directories(RmlDebugger PUBLIC
	Include/
	Source/
	../Freetype/include/
)

target_include_directories(RmlLua PUBLIC
	Include/
	Source/
	../Freetype/include/
	../Lua/src/
)

target_include_directories(RmlLottie PUBLIC
	Include/
	Source/
	../Rlottie/rlottie/inc/
)

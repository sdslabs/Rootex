###
# Checking for types
###
if(WIN32)
	include(CheckTypeSize)
	check_type_size(__int8 HAVE___INT8)
endif()

###
# Checking for Includes
###
include(CheckIncludeFile)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(windows.h HAVE_WINDOWS_H)
check_include_file(features.h HAVE_FEATURES_H)

###
# Checking for functions
###
include(CheckSymbolExists)

check_symbol_exists(_stat "sys/types.h;sys/stat.h" HAVE__STAT)
if(HAVE_TIME_H)
	check_symbol_exists(nanosleep "time.h" HAVE_NANOSLEEP)
endif()
if(HAVE_UNISTD_H)
	check_symbol_exists(usleep "unistd.h" HAVE_USLEEP)
	check_symbol_exists(stat "sys/types.h;sys/stat.h;unistd.h" HAVE_STAT)
endif()
if(HAVE_WINDOWS_H)
	check_symbol_exists(Sleep "windows.h" HAVE_SLEEP)
endif()

###
# Checking for __attribute__
###
include(CheckCSourceCompiles)
check_c_source_compiles("void  foo (int bar __attribute__((unused)) ) { }
	static void baz (void) __attribute__((unused));
	static void baz (void) { }
	int main(){}
	"
	HAVE___ATTRIBUTE__)

###
# Generate the config header
###
configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.cmake_in" "${CMAKE_CURRENT_LIST_DIR}/src/config.h")
#include_directories("${CMAKE_CURRENT_BINARY_DIR}")

###
# Build the library
###
add_definitions(-DHAVE_CONFIG_H=1)

# So our static library can be linked into shared libraries (for instance,
# gcc on amd64 needs -fPIC)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")

file(GLOB_RECURSE ALUT src/**.c)
file(GLOB_RECURSE ALUTH include/**.h src/**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/include
CACHE INTERNAL "")

add_library(alut SHARED ${ALUT} ${ALUTH})
set_property(TARGET alut PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set_target_properties(alut PROPERTIES LINKER_LANGUAGE C)

target_include_directories(alut PUBLIC
	include/
	src/
)

target_link_libraries(alut PUBLIC
	OpenAL
)

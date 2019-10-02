file(GLOB_RECURSE GameSource ./**.cpp)
file(GLOB_RECURSE GameHeaders ./**.h)

add_executable(Game ${GameSource} ${GameHeaders})

target_link_libraries(Game PUBLIC Rootex Vendor)
target_include_directories(Game PRIVATE
	../rootex/
	../vendor/Lua/src/
	../vendor/LuaBridges/Source/
)

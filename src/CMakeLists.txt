file(GLOB_RECURSE Sources **.cpp **.h)

add_library(Rootex STATIC ${Sources})

target_link_libraries(Rootex PUBLIC Vendor)

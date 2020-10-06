file(GLOB_RECURSE Tracy TracyClient.cpp)
file(GLOB_RECURSE TracyH ./**.h **.hpp)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/
CACHE INTERNAL "")

add_library(Tracy STATIC ${Tracy} ${TracyH})
set_target_properties(Tracy PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Tracy PUBLIC
    ./
)

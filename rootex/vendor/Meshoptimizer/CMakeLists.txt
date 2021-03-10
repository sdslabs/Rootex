file(GLOB_RECURSE Meshoptimizer meshoptimizer/src/**.cpp)
file(GLOB_RECURSE MeshoptimizerH meshoptimizer/src/**.h)

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}/meshoptimizer/src/
CACHE INTERNAL "")

add_library(Meshoptimizer STATIC ${Meshoptimizer} ${MeshoptimizerH})

target_include_directories(Meshoptimizer PRIVATE
	meshoptimizer/src/
)

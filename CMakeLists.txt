cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Setting output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "Bin")

# Project metadata
project(Rootex
    LANGUAGES CXX
    VERSION 1.0
    DESCRIPTION "Rootex Engine"
)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(game)

set(RootexIncludeDirectories
    src/
)

set(VendorIncludeDirectories 
    vendor/Lua/src/
    vendor/LuaBridge/Source/
)

target_include_directories(Rootex PUBLIC 
    ${RootexIncludeDirectories}
    ${VendorIncludeDirectories}
)

target_include_directories(Game PUBLIC 
    ${RootexIncludeDirectories}
    ${VendorIncludeDirectories}
)

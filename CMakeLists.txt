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

# RootexCore
file(GLOB_RECURSE RootexSource Source/**.cpp)
file(GLOB_RECURSE RootexHeaders Source/**.h)

add_library(RootexCore STATIC ${RootexSource} ${RootexHeaders})
target_include_directories(RootexCore PUBLIC Source/)

# Game project folder
add_subdirectory(Game)

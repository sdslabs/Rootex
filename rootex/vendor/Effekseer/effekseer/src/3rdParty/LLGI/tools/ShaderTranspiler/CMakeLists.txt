cmake_minimum_required(VERSION 3.0.0)
project(ShaderTranspiler)

add_executable(ShaderTranspiler main.cpp)

target_compile_features(ShaderTranspiler PUBLIC cxx_std_14)

target_include_directories(ShaderTranspiler PUBLIC ../ShaderTranspilerCore)

target_link_libraries(ShaderTranspiler PUBLIC ShaderTranspilerCore)

if(MSVC)
  target_compile_options(ShaderTranspiler PRIVATE /W4 /WX /wd"4100")
else()
  target_compile_options(ShaderTranspiler PRIVATE -Wall -Werror)
endif()
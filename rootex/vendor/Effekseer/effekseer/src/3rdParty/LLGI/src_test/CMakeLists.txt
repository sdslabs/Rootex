file(GLOB files *.h *.cpp)

add_executable(LLGI_Test ${files})

if(APPLE)

  find_library(COCOA_LIBRARY Cocoa)
  find_library(METAL_LIBRARY Metal)
  find_library(APPKIT_LIBRARY AppKit)
  find_library(METALKIT_LIBRARY MetalKit)
  find_library(QUARTZ_CORE_LIBRARY QuartzCore)

  set(EXTRA_LIBS ${COCOA_LIBRARY} ${APPKIT_LIBRARY} ${METAL_LIBRARY}
                 ${METALKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})
  target_link_libraries(LLGI_Test PRIVATE ${EXTRA_LIBS})

endif()

target_include_directories(LLGI_Test PUBLIC ../src/)

target_link_libraries(LLGI_Test PRIVATE LLGI)
target_compile_features(LLGI_Test PUBLIC cxx_std_14)

if(BUILD_VULKAN_COMPILER AND USE_THIRDPARTY_DIRECTORY)

  target_link_directories(LLGI_Test PRIVATE
                          ${LLGI_THIRDPARTY_LIBRARY_DIRECTORIES})
  target_link_libraries(LLGI_Test PRIVATE ${LLGI_THIRDPARTY_LIBRARIES})

endif()

if(MSVC)
  target_link_libraries(LLGI_Test PRIVATE)
elseif(APPLE)
  target_link_libraries(LLGI_Test PRIVATE)
else()
  find_package(Threads REQUIRED)
  target_link_libraries(LLGI_Test PRIVATE ${CMAKE_THREAD_LIBS_INIT} pthread X11
                                          X11-xcb)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

clang_format(LLGI_Test)

if(MSVC)
  target_compile_options(LLGI_Test PRIVATE /W4 /WX /wd"4100")
else()
  target_compile_options(LLGI_Test PRIVATE -Wall -Werror)
endif()

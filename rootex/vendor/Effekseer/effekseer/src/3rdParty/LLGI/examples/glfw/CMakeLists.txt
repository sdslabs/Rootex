add_executable(
  example_glfw
  main.cpp
)

target_include_directories(
  example_glfw
  PRIVATE
  ../../src/
)

target_link_libraries(
  example_glfw
  PRIVATE
  LLGI
  glfw
)

target_link_directories(example_glfw PRIVATE ${THIRDPARTY_LIBRARY_DIRECTORIES})
target_link_libraries(example_glfw PRIVATE ${THIRDPARTY_LIBRARIES})

if(WIN32)
  # None
elseif(APPLE)

  find_library(COCOA_LIBRARY Cocoa)
  find_library(METAL_LIBRARY Metal)
  find_library(APPKIT_LIBRARY AppKit)
  find_library(METALKIT_LIBRARY MetalKit)
  find_library(QUARTZ_CORE_LIBRARY QuartzCore)

  set(EXTRA_LIBS ${COCOA_LIBRARY} ${APPKIT_LIBRARY} ${METAL_LIBRARY} ${METALKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})
  target_link_libraries(example_glfw PRIVATE ${EXTRA_LIBS})

else()

  find_package(Threads REQUIRED)
  target_link_libraries(
    example_glfw
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    pthread
    X11
    X11-xcb
  )

endif()



if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  target_include_directories(example_glfw PRIVATE ${Vulkan_INCLUDE_DIRS})
  target_link_libraries(example_glfw PRIVATE ${Vulkan_LIBRARIES})

  target_include_directories(example_glfw PRIVATE ${LLGI_THIRDPARTY_INCLUDES})
  target_link_libraries(example_glfw PRIVATE ${LLGI_THIRDPARTY_LIBRARIES})
  target_link_directories(example_glfw PRIVATE ${LLGI_THIRDPARTY_LIBRARY_DIRECTORIES})
endif()

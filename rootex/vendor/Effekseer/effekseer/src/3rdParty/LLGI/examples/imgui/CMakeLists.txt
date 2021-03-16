add_executable(
  example_imgui
  main.cpp
)

target_include_directories(
  example_imgui
  PRIVATE
  ../../src/
  ../thirdparty/imgui/
)

target_link_libraries(
  example_imgui
  PRIVATE
  LLGI
  glfw
  imgui
  ImGuiPlatform
)

target_link_directories(example_imgui PRIVATE ${THIRDPARTY_LIBRARY_DIRECTORIES})
target_link_libraries(example_imgui PRIVATE ${THIRDPARTY_LIBRARIES})

if(WIN32)
  # None
elseif(APPLE)

  find_library(COCOA_LIBRARY Cocoa)
  find_library(METAL_LIBRARY Metal)
  find_library(APPKIT_LIBRARY AppKit)
  find_library(METALKIT_LIBRARY MetalKit)
  find_library(QUARTZ_CORE_LIBRARY QuartzCore)

  set(EXTRA_LIBS ${COCOA_LIBRARY} ${APPKIT_LIBRARY} ${METAL_LIBRARY} ${METALKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})
  target_link_libraries(example_imgui PRIVATE ${EXTRA_LIBS})

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
  set_target_properties(example_imgui PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")
else()

  find_package(Threads REQUIRED)
  target_link_libraries(
    example_imgui
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    pthread
    X11
    X11-xcb
  )

endif()

if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  target_include_directories(example_imgui PRIVATE ${Vulkan_INCLUDE_DIRS})
  target_link_libraries(example_imgui PRIVATE ${Vulkan_LIBRARIES})

  target_include_directories(example_imgui PRIVATE ${LLGI_THIRDPARTY_INCLUDES})
  target_link_libraries(example_imgui PRIVATE ${LLGI_THIRDPARTY_LIBRARIES})
  target_link_directories(example_imgui PRIVATE ${LLGI_THIRDPARTY_LIBRARY_DIRECTORIES})

endif()

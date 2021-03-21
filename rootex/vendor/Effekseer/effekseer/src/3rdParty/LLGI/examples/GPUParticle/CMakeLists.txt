add_executable(
  example_GPUParticle
  GPUParticle.h
  GPUParticle.cpp
  Main.cpp
)

target_include_directories(
  example_GPUParticle
  PRIVATE
  ../../src/
)

target_link_libraries(
  example_GPUParticle
  PRIVATE
  LLGI
  glfw
)

target_link_directories(example_GPUParticle PRIVATE ${THIRDPARTY_LIBRARY_DIRECTORIES})
target_link_libraries(example_GPUParticle PRIVATE ${THIRDPARTY_LIBRARIES})
target_compile_definitions(example_GPUParticle PRIVATE EXAMPLE_ASSET_DIR="${CMAKE_CURRENT_LIST_DIR}")

if(WIN32)
  # None
elseif(APPLE)

  find_library(COCOA_LIBRARY Cocoa)
  find_library(METAL_LIBRARY Metal)
  find_library(APPKIT_LIBRARY AppKit)
  find_library(METALKIT_LIBRARY MetalKit)
  find_library(QUARTZ_CORE_LIBRARY QuartzCore)

  set(EXTRA_LIBS ${COCOA_LIBRARY} ${APPKIT_LIBRARY} ${METAL_LIBRARY} ${METALKIT_LIBRARY} ${QUARTZ_CORE_LIBRARY})
  target_link_libraries(example_GPUParticle PRIVATE ${EXTRA_LIBS})

else()

  find_package(Threads REQUIRED)
  target_link_libraries(
    example_GPUParticle
    PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    pthread
    X11
    X11-xcb
  )

endif()


if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  target_include_directories(example_GPUParticle PRIVATE ${Vulkan_INCLUDE_DIRS})
  target_link_libraries(example_GPUParticle PRIVATE ${Vulkan_LIBRARIES})

  target_include_directories(example_GPUParticle PRIVATE ${LLGI_THIRDPARTY_INCLUDES})
  target_link_libraries(example_GPUParticle PRIVATE ${LLGI_THIRDPARTY_LIBRARIES})
  target_link_directories(example_GPUParticle PRIVATE ${LLGI_THIRDPARTY_LIBRARY_DIRECTORIES})
endif()

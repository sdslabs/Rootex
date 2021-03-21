set(
  srcs
  ImGuiPlatform.h
)

if(WIN32)
  list(
    APPEND
    srcs
    ImGuiPlatformDX12.cpp
  )
endif()

if(APPLE)
  list(
    APPEND
    srcs
    ImGuiPlatformMetal.mm
  )
endif()

if(BUILD_VULKAN)
  list(
    APPEND
    srcs
    ImGuiPlatformVulkan.cpp
  )
endif()

add_library(
  ImGuiPlatform
  STATIC
  ${srcs}
)

target_compile_features(ImGuiPlatform PUBLIC cxx_std_14)

target_include_directories(
  ImGuiPlatform
  PRIVATE
  ../../src/
  ../thirdparty/imgui/
)

target_include_directories(
  ImGuiPlatform
  PUBLIC
  ./
)

target_link_libraries(
  ImGuiPlatform
  PRIVATE
  LLGI
  imgui
)

if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  target_include_directories(ImGuiPlatform PRIVATE ${Vulkan_INCLUDE_DIRS})
endif()

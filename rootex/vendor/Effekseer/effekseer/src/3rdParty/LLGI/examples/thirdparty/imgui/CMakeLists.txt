enable_language(CXX)

# imgui
list(
  APPEND srcs
  imgui.h
  imgui_impl_glfw.h
  imgui.cpp
  imgui_draw.cpp
  imgui_widgets.cpp
  imgui_impl_glfw.cpp
)

if(WIN32)
  list(
    APPEND
    srcs
    imgui_impl_dx12.cpp
  )

elseif(APPLE)
  list(
    APPEND
    srcs
    imgui_impl_metal.mm
  )
endif()

if(BUILD_VULKAN)
  list(
    APPEND
    srcs
    imgui_impl_vulkan.cpp
  )
endif()

add_library(imgui STATIC ${srcs})

target_include_directories(
  imgui
  PRIVATE
  ../glfw/include
)

set_property(TARGET imgui PROPERTY POSITION_INDEPENDENT_CODE ON)

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")
  set_target_properties(imgui PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")
endif()

if(BUILD_VULKAN)
  find_package(Vulkan REQUIRED)
  target_include_directories(imgui PRIVATE ${Vulkan_INCLUDE_DIRS})
endif()

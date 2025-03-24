cmake_minimum_required(VERSION 3.30)
project(navier-stokes LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

option(ENABLE_VK_VALIDATIONS "Enable Vulkan validation layers" OFF)

add_executable(navier-stokes-vk-headless)
target_sources(navier-stokes-vk-headless
  PRIVATE
  main.cpp vma.cpp
)
target_link_libraries(navier-stokes-vk-headless
  PRIVATE
  Vulkan::Vulkan
)
if(ENABLE_VK_VALIDATIONS)
  message(STATUS "Vulkan validations enabled")
  target_compile_definitions(navier-stokes-vk-headless PRIVATE NS_VK_VALIDATIONS)
endif()

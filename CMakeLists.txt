cmake_minimum_required(VERSION 3.30)
project(echo_uring LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 3.2 REQUIRED CONFIG)
find_package(Vulkan REQUIRED)

# have multiple libraries
# feature?: ship some of them for a target
# and on runtime determine what to use?
add_library(generic_solver)
target_sources(generic_solver
  PRIVATE
  solver/generic.cpp
)

add_executable(navier-stokes-headless)
target_sources(navier-stokes-headless
  PRIVATE
  headless.cpp
)
target_link_libraries(navier-stokes-headless
  PRIVATE
  generic_solver
)

add_executable(navier-stokes-render)
target_sources(navier-stokes-render
  PRIVATE
  render.cpp
)
target_link_libraries(navier-stokes-render
  PRIVATE
  SDL3::SDL3
  Vulkan::Vulkan
)

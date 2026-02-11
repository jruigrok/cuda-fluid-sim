cmake_minimum_required(VERSION 3.10)

project(CudaFluidSim)

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Specify the executable and link SFML libraries
add_executable(cuda_fluid_sim 
    src/main.cpp
    src/Cell.cpp
    src/Grid.cpp
    src/ViewPort.cpp
)
target_include_directories(cuda_fluid_sim PRIVATE src)
target_link_libraries(cuda_fluid_sim sfml-graphics sfml-window sfml-system)

set(CIRCLE_IMG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/artifacts/circle.png")
add_compile_definitions(CIRCLE_IMG_PATH="${CIRCLE_IMG_PATH}")

message("Build type: ${CMAKE_BUILD_TYPE}")

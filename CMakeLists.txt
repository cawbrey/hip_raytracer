cmake_minimum_required(VERSION 4.0)
project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3")

find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

add_executable(raytracer src/main.cpp)

target_include_directories(raytracer PUBLIC include)
target_link_libraries(raytracer PRIVATE SDL2::SDL2 glm::glm)
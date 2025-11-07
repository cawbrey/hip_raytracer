cmake_minimum_required(VERSION 4.0)
project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(raytracer src/main.cpp)

target_include_directories(raytracer PUBLIC include)
target_link_libraries(raytracer PRIVATE SDL2::SDL2)
cmake_minimum_required(VERSION 4.0)
project(hip_raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(hip_raytracer src/main.cpp)
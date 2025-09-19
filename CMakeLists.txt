cmake_minimum_required(VERSION 4.0)
project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(raytracer src/main.cpp
        src/hello_world.cpp
        src/math.cpp
)
target_include_directories(raytracer PUBLIC include)

option(BUILD_TESTS "Build Unit Tests" ON)
if (BUILD_TESTS)
    enable_testing()

    find_package(GTest REQUIRED)
    find_package(Threads REQUIRED)

    add_executable(raytracer_test
            test/math_test.cpp)
    target_link_libraries(raytracer_test PRIVATE GTest::gtest_main Threads::Threads)
    include(GoogleTest)
    gtest_discover_tests(raytracer_test)
endif ()
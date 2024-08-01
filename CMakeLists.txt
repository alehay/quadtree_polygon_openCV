cmake_minimum_required(VERSION 3.10)
project(quadtree_polygon_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenCV 4.9.0 REQUIRED)

# Function to set compile options
function(set_strict_compile_options target)
    target_compile_options(${target} PRIVATE
        -pedantic-errors
        -Wall
        -Wextra
        -pedantic
        -Wpedantic
        -Wfloat-equal
        -Wpointer-arith
        -Wstrict-overflow=5
        -Wwrite-strings
        -Waggregate-return
        -Wswitch-default
        -Wswitch-enum
        -Wconversion
        -Wunreachable-code
    )
endfunction()

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
set_strict_compile_options(${PROJECT_NAME})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# ... [ASAN and UBSAN settings remain unchanged]

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(" \n ——————————► DEBUG ◄——————————")
    add_compile_options(-g3 -O0)
endif()

# Release configuration
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Print build type and OpenCV version
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

# Google Test setup
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests test.cpp)
target_include_directories(tests PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tests ${OpenCV_LIBS} gtest_main)
set_strict_compile_options(tests)

include(GoogleTest)
gtest_discover_tests(tests)

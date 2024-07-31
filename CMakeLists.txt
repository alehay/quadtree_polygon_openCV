cmake_minimum_required(VERSION 3.10)
project(quadtree_polygon_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenCV 4.9.0 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

IF(ASAN)
    message("ASAN sanitazer ON")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address ")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address  ")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address ")
    add_compile_options( -fsanitize=address  ) 
ENDIF()

IF(UBSAN) 
#https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
    message("UB sanitazer ON")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=undefined")
    add_compile_options(-fsanitize=undefined  -fsanitize=signed-integer-overflow,null,alignment -fsanitize-recover=all) 
ENDIF()


IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message(" \n ——————————► DEBUG ◄——————————")
  # debugging information in a binary file
  add_compile_options( -g3 -O0 ) 

  #add_compile_options(-Werror) # turnsform  warnings into errors
  
  # Warnings include  
  add_compile_options(
    -pedantic-errors
    -Wall
    -Wextra
    -pedantic
    -Wpedantic
    -std=c++17
    -Wfloat-equal
    -Wpointer-arith
    -Wstrict-overflow=5 
    -Wwrite-strings
    -Waggregate-return
    -Wswitch-default -Wswitch-enum
    -Wconversion
    -Wunreachable-code
    #-fstack-clash-protection -fstack-protector	-fcf-protection
    #-fasynchronous-unwind-tables


    #-Wcast-align
    #-Wcast-qual
    #-Wctor-dtor-privacy
    #-Wduplicated-branches
    #-Wduplicated-cond

    #-Wextra-semi
    #-Wlogical-op
    #-Wnon-virtual-dtor
    #-Wold-style-cast
  
    #-Woverloaded-virtual
    #-Wredundant-decls
    #-Wsign-conversion
    #-Wsign-promo
    #-Weffc++
    #-Woverloaded-virtual
    )
ENDIF()



# Release configuration
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Set default build type to Release if not specified

# Print build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Print OpenCV version
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

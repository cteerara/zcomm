cmake_minimum_required(VERSION 3.16)
project(zcomm)

set(CMAKE_CXX_STANDARD 17)
find_package(MPI REQUIRED)


# Build zcomm_wrapper class
if(DEFINED ENV{ZOLTAN_DIR})
    set(ZOLTAN_DIR $ENV{ZOLTAN_DIR})
    message(STATUS "ZOLTAN_DIR is found at ${ZOLTAN_DIR}")
else()
    message(FATAL_ERROR "ZOLTAN_DIR is NOT found!
    Please set the environment variable ZOLTAN_DIR to the directory where zoltan is built.")
endif()
find_library(ZOLTAN_LIB zoltan ${ZOLTAN_DIR}/lib)


# Build executable
add_executable(test test/main.cpp)
find_library(TEST_LIBRARY cmake_testapp_lib lib)
target_include_directories(test PUBLIC include ${ZOLTAN_DIR}/include)
target_link_libraries(test LINK_PUBLIC ${ZOLTAN_LIB})


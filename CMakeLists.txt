cmake_minimum_required(VERSION 3.16)
project(zcomm VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
find_package(MPI REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
        ${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export the library for use in other projects
install(EXPORT ${PROJECT_NAME}Config
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}
)

# Create a package config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}
)

# Build demo
option(BUILD_DEMO "Build the demo executable" OFF)
if(BUILD_DEMO)

    # Message status
    message(STATUS "Building the demo executable")

    # Build zcomm_wrapper class
    if(DEFINED ENV{ZOLTAN_DIR})
        set(ZOLTAN_DIR $ENV{ZOLTAN_DIR})
        message(STATUS "ZOLTAN_DIR is found at ${ZOLTAN_DIR}")
    else()
        message(FATAL_ERROR "ZOLTAN_DIR is NOT found!
    Please set the environment variable ZOLTAN_DIR to the directory where zoltan is built.")
    endif()

    # Find zoltan library
    find_library(ZOLTAN_LIB zoltan ${ZOLTAN_DIR}/lib)

    # Build executable
    add_executable(demo demo/demo.cpp)
    target_include_directories(demo PUBLIC ${ZOLTAN_DIR}/include)
    target_link_libraries(demo LINK_PUBLIC ${ZOLTAN_LIB} ${PROJECT_NAME})

endif()

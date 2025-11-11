cmake_minimum_required(VERSION 3.22.1)

project("rnbo_test")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent for Oboe
include(FetchContent)
FetchContent_Declare(
    oboe
    GIT_REPOSITORY https://github.com/google/oboe.git
    GIT_TAG 1.8.1
)
FetchContent_MakeAvailable(oboe)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rnbo)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rnbo/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rnbo/src)

# Find all RNBO source files
file(GLOB RNBO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rnbo/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rnbo/common/*.cpp
)

# Find the main .cpp file (RNBO export)
file(GLOB MAIN_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create shared library
add_library(${CMAKE_PROJECT_NAME} SHARED
    ${MAIN_CPP_FILES}
    ${RNBO_SOURCES}
    MSPLoader.cpp
    BufferLoader.cpp
    native-lib.cpp
)

# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    android
    log
    oboe
)

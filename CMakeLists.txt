cmake_minimum_required(VERSION 3.10)
project(ThreadPool VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

# Add main subdirectory
add_subdirectory(sw)

# Add formatting utilities
include(cmake/clang_format.cmake)

# Enable testing
#enable_testing()

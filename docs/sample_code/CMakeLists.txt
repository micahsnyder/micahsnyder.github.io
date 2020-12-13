# Copyright (C) 2020 Micah Snyder.

cmake_minimum_required( VERSION 3.14 )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

project( SampleCode
         VERSION "0.1.0"
         DESCRIPTION "Sample code for: Error Handling in C" )

set(PACKAGE_URL "https://blog.micahsnyder.dev/")

#
# Define LINUX because CMake only defines UNIX
#
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(LINUX 1)
endif()

# Check for valgrind. If it exists, we'll enable extra tests that use valgrind.
if(LINUX)
    find_package(Valgrind)
endif()

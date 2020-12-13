# Copyright (C) 2020 Micah Snyder.

cmake_minimum_required( VERSION 3.14 )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

project( SampleCode
         VERSION "0.1.0"
         DESCRIPTION "Sample code for: Error Handling in C" )

set(PACKAGE_URL "https://blog.micahsnyder.dev/")

# Define LINUX because CMake only defines UNIX
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(LINUX 1)
endif()


set(WARNCFLAGS)
if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set(WARNCFLAGS    /WX)
else()
    if(ENABLE_WERROR)
        set(WARNCFLAGS    -Wall -Wextra)
    else()
        set(WARNCFLAGS    -Wall -Wextra -Werror)
    endif()
endif()
set_directory_properties(COMPILE_FLAGS "${WARNCFLAGS}")

# Check for valgrind. If it exists, we'll enable extra tests that use valgrind.
if(LINUX)
    find_package(Valgrind)
endif()

include(CTest)

add_library(sample_test)
target_sources(sample_test
    PRIVATE sample_test.c
    PUBLIC  sample_test.h)

add_executable(01_multi_return)
target_sources(01_multi_return
    01-multi-return.c)
target_link_libraries(01_multi_return PRIVATE sample_test)


#
# The Tests
# ~~~~~~~~~
#
# Run all tests with: `ctest`
#                 or: `ctest -V` for verbose output
#
# Run a specific test like this:
#                     `ctest -V -R 01_multi_return
#
add_test(
    NAME 01_multi_return_test
    COMMAND $<TARGET_FILE:01_multi_return>)
if(Valgrind_FOUND)
    add_test(
        NAME 01_multi_return_valgrind_test
        COMMAND ${Valgrind_EXECUTABLE} --leak-check=full;--show-leak-kinds=all;$<TARGET_FILE:01_multi_return>)
endif()
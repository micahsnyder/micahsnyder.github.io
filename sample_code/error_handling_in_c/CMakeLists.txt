# Copyright (C) 2020 Micah Snyder.

#
# Build with:
# ```sh
#     mkdir build
#     cd build
#     cmake ..
#     cmake --build .
# ```
#
# Run all tests with: `ctest`
#                 or: `ctest -V` for verbose output
#
# Run a specific test like this:
#                     `ctest -V -R 01_multi_return
#

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

# Disable optimizations
#
# Get rid of any previous optimization flag settings...
string(REGEX REPLACE "(\-O[011123456789])" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
# ...And substitute our own.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

# Enable Debug mode
set(CMAKE_BUILD_TYPE Debug)

# Enable lots of warnings
#
set(WARNCFLAGS)
if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set(WARNCFLAGS    /WX)
else()
    if(ENABLE_WERROR)
        set(WARNCFLAGS -Wall -Wextra)
    else()
        set(WARNCFLAGS -Wall -Wextra -Werror)
    endif()
endif()
set_directory_properties(PROPERTIES COMPILE_FLAGS "${WARNCFLAGS}")

# Check for valgrind. If it exists, we'll enable extra tests that use valgrind.
if(LINUX)
    #
    # Find the Valgrind program.
    #
    # If found, will set: Valgrind_FOUND, Valgrind_VERSION, and Valgrind_EXECUTABLE
    #
    # If you have a custom install location for Valgrind, you can provide a hint
    # by settings -DValgrind_HOME=<directory containing valgrind>
    #

    find_program(Valgrind_EXECUTABLE valgrind
        HINTS "${Valgrind_HOME}"
        PATH_SUFFIXES "bin"
    )
    if(Valgrind_EXECUTABLE)
        execute_process(COMMAND "${Valgrind_EXECUTABLE}" --version
            OUTPUT_VARIABLE Valgrind_VERSION_OUTPUT
            ERROR_VARIABLE  Valgrind_VERSION_ERROR
            RESULT_VARIABLE Valgrind_VERSION_RESULT
        )
        if(NOT ${Valgrind_VERSION_RESULT} EQUAL 0)
            message(STATUS "Valgrind not found: Failed to determine version.")
            unset(Valgrind_EXECUTABLE)
        else()
            string(REGEX
                MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?(-nightly)?"
                Valgrind_VERSION "${Valgrind_VERSION_OUTPUT}"
            )
            set(Valgrind_VERSION "${Valgrind_VERSION}")
            set(Valgrind_FOUND 1)
            message(STATUS "Valgrind found: ${Valgrind_EXECUTABLE}, ${Valgrind_VERSION}")
        endif()

        mark_as_advanced(Valgrind_EXECUTABLE Valgrind_VERSION)
    else()
        message(STATUS "Valgrind not found.")
    endif()
endif()

include(CTest)

add_library(sample_test)
target_sources(sample_test
    PRIVATE sample_test.c
    PUBLIC  sample_test.h)

#
# The Tests
#
set(SAMPLES
    01_multi_return
    02_do_while
    03_goto_done
    04_goto_done_with_macros
    05_else_if
)

foreach(SAMPLE ${SAMPLES})
    add_executable(${SAMPLE})
    target_sources(${SAMPLE}
        PRIVATE ${SAMPLE}.c)
    target_link_libraries(${SAMPLE} PRIVATE sample_test)

    add_test(
        NAME ${SAMPLE}_test
        COMMAND $<TARGET_FILE:${SAMPLE}>)
    if(Valgrind_FOUND)
        add_test(
            NAME ${SAMPLE}_valgrind_test
            COMMAND ${Valgrind_EXECUTABLE} --leak-check=full;--show-leak-kinds=all;$<TARGET_FILE:${SAMPLE}>)
    endif()
endforeach()

string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type)
message(STATUS "Configuration Options Summary --
    Host system:            ${CMAKE_HOST_SYSTEM}
    Target system:          ${CMAKE_SYSTEM}
    Compiler:
        Build type:         ${CMAKE_BUILD_TYPE}
        C compiler:         ${CMAKE_C_COMPILER}
        CFLAGS:             ${CMAKE_C_FLAGS_${_build_type}} ${CMAKE_C_FLAGS}
        WARNCFLAGS:         ${WARNCFLAGS}"
)

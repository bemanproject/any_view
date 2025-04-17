# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.25)

project(
    beman.any_view
    DESCRIPTION "A generalized type-erased view with customizable properties"
    LANGUAGES CXX
)

# [CMAKE.SKIP_TESTS]
option(
    BEMAN_ANY_VIEW_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ON. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

option(
    BEMAN_ANY_VIEW_BUILD_EXAMPLES
    "Enable building examples. Default: ON. Values: {ON, OFF}."
    ${PROJECT_IS_TOP_LEVEL}
)

set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(GNUInstallDirs)

add_library(beman.any_view INTERFACE)
add_library(beman::any_view ALIAS beman.any_view)
target_compile_features(beman.any_view INTERFACE cxx_std_20)
target_include_directories(
    beman.any_view
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(
    TARGETS beman.any_view
    EXPORT ${TARGETS_EXPORT_NAME}
    DESTINATION
    $<$<CONFIG:Debug>:debug/>${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}
    FILES_MATCHING
    PATTERN "${CMAKE_CURRENT_SOURCE_DIR}/include/beman/any_view/*.hpp"
)

macro(beman_add_executable)
    set(options)
    set(oneValueArgs CATEGORY TARGET)
    set(multiValueArgs SOURCES LIBRARIES)

    cmake_parse_arguments(
        beman_executable
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    set(target
        "beman.any_view.${beman_executable_CATEGORY}.${beman_executable_TARGET}"
    )

    if(NOT beman_executable_SOURCES)
        set(beman_executable_SOURCES "${beman_executable_TARGET}.cpp")
    endif()

    add_executable(${target})
    target_sources(${target} PRIVATE ${beman_executable_SOURCES})
    target_link_libraries(
        ${target}
        PRIVATE beman::any_view ${beman_executable_LIBRARIES}
    )
endmacro()

if(BEMAN_ANY_VIEW_BUILD_TESTS)
    enable_testing()

    # Fetch GoogleTest
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        EXCLUDE_FROM_ALL
    )
    set(INSTALL_GTEST OFF) # Disable GoogleTest installation
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests/beman/any_view)
endif()

if(BEMAN_ITERATOR_INTERFACE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

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

# [CPP.NO_FLAG_FORKING]
set(BEMAN_ANY_VIEW_DESIGN
    "FLAGS"
    CACHE STRING
    "Enable alternative design for any_view_options. Default: FLAGS. Values: { FLAGS, TRAITS, NAMED }."
)

set(BEMAN_ANY_VIEW_OPTION
    "COPYABLE"
    CACHE STRING
    "Enable opt-in option for any_view. Default: COPYABLE. Values: { COPYABLE, MOVE_ONLY }."
)

set(BEMAN_ANY_VIEW_USE_FLAGS OFF)
set(BEMAN_ANY_VIEW_USE_TRAITS OFF)
set(BEMAN_ANY_VIEW_USE_NAMED OFF)
set(BEMAN_ANY_VIEW_USE_COPYABLE OFF)
set(BEMAN_ANY_VIEW_USE_MOVE_ONLY OFF)

if(BEMAN_ANY_VIEW_DESIGN STREQUAL "FLAGS")
    set(BEMAN_ANY_VIEW_USE_FLAGS ON)
elseif(BEMAN_ANY_VIEW_DESIGN STREQUAL "TRAITS")
    set(BEMAN_ANY_VIEW_USE_TRAITS ON)
elseif(BEMAN_ANY_VIEW_DESIGN STREQUAL "NAMED")
    set(BEMAN_ANY_VIEW_USE_NAMED ON)
else()
    message(
        FATAL_ERROR
        "BEMAN_ANY_VIEW_DESIGN must be one of { FLAGS, TRAITS, NAMED }; got ${BEMAN_ANY_VIEW_DESIGN}"
    )
endif()

if(BEMAN_ANY_VIEW_OPTION STREQUAL "COPYABLE")
    set(BEMAN_ANY_VIEW_USE_COPYABLE ON)
elseif(BEMAN_ANY_VIEW_OPTION STREQUAL "MOVE_ONLY")
    set(BEMAN_ANY_VIEW_USE_MOVE_ONLY ON)
else()
    message(
        FATAL_ERROR
        "BEMAN_ANY_VIEW_OPTION must be one of { COPYABLE, MOVE_ONLY }; got ${BEMAN_ANY_VIEW_OPTION}"
    )
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/include/beman/any_view/config.hpp.in"
    "${PROJECT_BINARY_DIR}/include/beman/any_view/config.hpp"
    @ONLY
)

include(FetchContent)
include(GNUInstallDirs)

add_library(beman.any_view INTERFACE)
add_library(beman::any_view ALIAS beman.any_view)
# TODO: implement non-template friend function and remove this suppression
target_compile_options(
    beman.any_view
    INTERFACE $<$<CXX_COMPILER_ID:GNU>:-Wno-non-template-friend>
)
target_include_directories(
    beman.any_view
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
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

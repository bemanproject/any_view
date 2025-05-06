# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.25)

# [CMAKE.PROJECT_NAME]
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

# [CMAKE.SKIP_EXAMPLES]
option(
    BEMAN_ANY_VIEW_BUILD_EXAMPLES
    "Enable building examples. Default: ON. Values: {ON, OFF}."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CPP.NO_FLAG_FORKING]
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS
        "[[msvc::no_unique_address]]"
        CACHE STRING
        "Name of C++ attribute for empty base optimization of data members"
    )
else()
    set(BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS
        "[[no_unique_address]]"
        CACHE STRING
        "Name of C++ attribute for empty base optimization of data members"
    )
endif()

configure_file(
    include/beman/any_view/config.hpp.in
    include/beman/any_view/config.hpp
    @ONLY
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

# [CMAKE.LIBRARY_NAME]
add_library(beman.any_view INTERFACE)
# [CMAKE.LIBRARY_ALIAS]
add_library(beman::any_view ALIAS beman.any_view)
target_include_directories(
    beman.any_view
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_sources(
    beman.any_view
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/beman/any_view/detail/any_iterator.hpp
            include/beman/any_view/detail/concepts.hpp
            include/beman/any_view/detail/intrusive_small_ptr.hpp
            include/beman/any_view/detail/iterator_adaptor.hpp
            include/beman/any_view/detail/iterator_interface.hpp
            include/beman/any_view/detail/type_traits.hpp
            include/beman/any_view/detail/utility.hpp
            include/beman/any_view/detail/view_adaptor.hpp
            include/beman/any_view/detail/view_interface.hpp
            include/beman/any_view/any_view_options.hpp
            include/beman/any_view/any_view.hpp
            include/beman/any_view/concepts.hpp
)

include(GNUInstallDirs)

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
    PATTERN "${CMAKE_CURRENT_BINARY_DIR}/include/beman/any_view/*.hpp"
)

function(beman_add_executable)
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

    # [CMAKE.TARGET_NAMES]
    set(category beman.any_view.${beman_executable_CATEGORY})
    set(target ${category}.${beman_executable_TARGET})

    add_executable(${target})
    # [CMAKE.PASSIVE_PROJECTS]
    # set features on executables, not on interface library
    target_compile_features(${target} PRIVATE cxx_std_20)
    target_sources(${target} PRIVATE ${beman_executable_SOURCES})
    target_link_libraries(
        ${target}
        PRIVATE beman::any_view ${beman_executable_LIBRARIES}
    )

    if(NOT TARGET ${category})
        add_custom_target(${category})
    endif()

    add_dependencies(${category} ${target})
endfunction()

if(BEMAN_ANY_VIEW_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/beman/any_view)
endif()

if(BEMAN_ANY_VIEW_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

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
    "Enable building tests and test infrastructure. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.SKIP_EXAMPLES]
option(
    BEMAN_ANY_VIEW_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

option(
    BEMAN_ANY_VIEW_INSTALL_CONFIG_FILE_PACKAGE
    "Enable creating and installing a CMake config-file package. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.LIBRARY_NAME]
add_library(beman.any_view INTERFACE)
# [CMAKE.LIBRARY_ALIAS]
add_library(beman::any_view ALIAS beman.any_view)

set_target_properties(
    beman.any_view
    PROPERTIES
        #
        VERIFY_INTERFACE_HEADER_SETS ON
        EXPORT_NAME any_view
)

target_sources(
    beman.any_view
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS #
            include
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
    #
    TARGETS beman.any_view
    EXPORT beman.any_view-targets
    FILE_SET HEADERS
)

if(BEMAN_ANY_VIEW_INSTALL_CONFIG_FILE_PACKAGE)
    install(
        EXPORT beman.any_view-targets
        FILE beman.any_view-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/beman.any_view
        NAMESPACE beman::
    )
endif()

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

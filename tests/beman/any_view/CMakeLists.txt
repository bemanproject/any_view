# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(GTest REQUIRED)
set(BENCHMARK_ENABLE_TESTING OFF)
find_package(benchmark REQUIRED)

include(GoogleTest)

set(BENCHMARK_DETAIL_SOURCES
    detail/eager.cpp
    detail/fused.cpp
    detail/lazy.cpp
    detail/products.cpp
    detail/reserved.cpp
)

function(beman_add_benchmark name)
    set(target beman.any_view.benchmarks.${name})
    add_executable(${target})
    target_compile_features(${target} PRIVATE cxx_std_20)
    target_sources(${target} PRIVATE ${name}.benchmark.cpp ${ARGN})
    target_link_libraries(
        ${target}
        PRIVATE beman::any_view benchmark::benchmark
    )

    if(NOT TARGET beman.any_view.benchmarks)
        add_custom_target(beman.any_view.benchmarks)
    endif()

    add_dependencies(beman.any_view.benchmarks ${target})
endfunction()

function(beman_add_tests)
    foreach(name ${ARGN})
        set(target beman.any_view.tests.${name})
        add_executable(${target})
        target_compile_features(${target} PRIVATE cxx_std_20)
        target_sources(${target} PRIVATE ${name}.test.cpp)
        target_link_libraries(
            ${target}
            PRIVATE beman::any_view GTest::gtest GTest::gtest_main
        )
        gtest_discover_tests(${target})
    endforeach()
endfunction()

beman_add_benchmark(all ${BENCHMARK_DETAIL_SOURCES})
beman_add_benchmark(take ${BENCHMARK_DETAIL_SOURCES})

beman_add_tests(concepts constexpr iterator sfinae type_traits)

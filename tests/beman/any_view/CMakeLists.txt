# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(GTest REQUIRED)
set(BENCHMARK_ENABLE_TESTING OFF)
find_package(benchmark REQUIRED)

include(GoogleTest)

function(beman_add_benchmark name)
    beman_add_executable(
        TARGET ${name}
        SOURCES ${name}.benchmark.cpp ${ARGN}
        CATEGORY benchmarks
        LIBRARIES benchmark::benchmark
    )
endfunction()

function(beman_add_tests)
    foreach(name ${ARGN})
        beman_add_executable(
            TARGET ${name}
            SOURCES ${name}.test.cpp
            CATEGORY tests
            LIBRARIES GTest::gtest GTest::gtest_main
        )
        gtest_discover_tests(beman.any_view.tests.${name})
    endforeach()
endfunction()

beman_add_benchmark(
    all
    detail/eager.cpp
    detail/fused.cpp
    detail/lazy.cpp
    detail/products.cpp
    detail/reserved.cpp
)
beman_add_benchmark(
    take
    detail/eager.cpp
    detail/fused.cpp
    detail/lazy.cpp
    detail/products.cpp
    detail/reserved.cpp
)

beman_add_tests(
    concepts
    constexpr
    sfinae
    type_traits
)

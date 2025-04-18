# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Fetch Benchmark
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.2
    EXCLUDE_FROM_ALL
)
set(BENCHMARK_ENABLE_TESTING OFF)
# Fetch GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    EXCLUDE_FROM_ALL
)
set(INSTALL_GTEST OFF) # Disable GoogleTest installation
FetchContent_MakeAvailable(benchmark googletest)

include(GoogleTest)

function(beman_add_benchmark)
    beman_add_executable(${ARGN} CATEGORY benchmarks LIBRARIES benchmark::benchmark)
endfunction()

function(beman_add_test)
    beman_add_executable(${ARGN} CATEGORY tests LIBRARIES GTest::gtest GTest::gtest_main)
    gtest_discover_tests(${target})
endfunction()

beman_add_benchmark(TARGET all SOURCES all.benchmark.cpp detail/eager.cpp detail/fused.cpp detail/lazy.cpp detail/products.cpp detail/reserved.cpp)
beman_add_benchmark(TARGET take SOURCES take.benchmark.cpp detail/eager.cpp detail/fused.cpp detail/lazy.cpp detail/products.cpp detail/reserved.cpp)

beman_add_test(TARGET concepts SOURCES concepts.test.cpp)
beman_add_test(TARGET constexpr SOURCES constexpr.test.cpp)
beman_add_test(TARGET sfinae SOURCES sfinae.test.cpp)
beman_add_test(TARGET type_traits SOURCES type_traits.test.cpp)

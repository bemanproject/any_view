// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "detail/eager.hpp"
#include "detail/fused.hpp"
#include "detail/lazy.hpp"
#include "detail/products.hpp"
#include "detail/reserved.hpp"

#include <benchmark/benchmark.h>

constexpr auto max_size = 1 << 18;

const auto global_products = generate_random_products(max_size);

inline void use(std::string_view name) {
    auto front = name.front();
    auto back  = name.back();
    benchmark::DoNotOptimize(front);
    benchmark::DoNotOptimize(back);
}

static void BM_all_eager(benchmark::State& state) {
    const auto size  = state.range(0);
    const auto begin = global_products.begin();

    eager::database db{.products = {begin, begin + size}};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

static void BM_all_fused(benchmark::State& state) {
    const auto size  = state.range(0);
    const auto begin = global_products.begin();

    fused::database db{.products = {begin, begin + size}};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

static void BM_all_lazy(benchmark::State& state) {
    const auto size  = state.range(0);
    const auto begin = global_products.begin();

    lazy::database db{.products = {begin, begin + size}};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

static void BM_all_reserved(benchmark::State& state) {
    const auto size  = state.range(0);
    const auto begin = global_products.begin();

    reserved::database db{.products = {begin, begin + size}};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

BENCHMARK(BM_all_eager)->RangeMultiplier(2)->Range(1 << 10, max_size);
BENCHMARK(BM_all_fused)->RangeMultiplier(2)->Range(1 << 10, max_size);
BENCHMARK(BM_all_lazy)->RangeMultiplier(2)->Range(1 << 10, max_size);
BENCHMARK(BM_all_reserved)->RangeMultiplier(2)->Range(1 << 10, max_size);

BENCHMARK_MAIN();

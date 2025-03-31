// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "detail/eager.hpp"
#include "detail/fused.hpp"
#include "detail/lazy.hpp"
#include "detail/products.hpp"

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
    eager::database db{global_products | std::views::take(state.range(0)) | std::ranges::to<std::vector>()};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

static void BM_all_fused(benchmark::State& state) {
    fused::database db{global_products | std::views::take(state.range(0)) | std::ranges::to<std::vector>()};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

static void BM_all_lazy(benchmark::State& state) {
    lazy::database db{global_products | std::views::take(state.range(0)) | std::ranges::to<std::vector>()};

    for (auto _ : state) {
        for (std::string_view name : db.get_products({.min_quantity = 10})) {
            use(name);
        }
    }
}

BENCHMARK(BM_all_eager)->RangeMultiplier(2)->Range(1 << 10, max_size);
BENCHMARK(BM_all_fused)->RangeMultiplier(2)->Range(1 << 10, max_size);
BENCHMARK(BM_all_lazy)->RangeMultiplier(2)->Range(1 << 10, max_size);

BENCHMARK_MAIN();

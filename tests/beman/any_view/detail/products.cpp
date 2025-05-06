// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "products.hpp"

#include <random>

auto generate_random_products(std::size_t count) -> std::vector<product_t> {
    std::vector<product_t> results;
    results.reserve(count);

    constexpr char alphanum[] = "0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz";

    std::mt19937                                             char_rng;
    std::uniform_int_distribution<std::mt19937::result_type> char_dist(0, sizeof(alphanum) - 1);

    std::mt19937                                             len_rng;
    std::uniform_int_distribution<std::mt19937::result_type> len_dist(1, 30);

    const auto gen_next_str = [&]() {
        std::string str;
        str.reserve(len_dist(len_rng));

        for (std::size_t i = 0; i < str.capacity(); ++i) {
            str.push_back(alphanum[char_dist(char_rng)]);
        }

        return str;
    };

    std::mt19937                       w_rng;
    std::uniform_int_distribution<int> w_dist(0, 100);

    const auto gen_size = [&]() -> std::size_t { return w_dist(w_rng); };

    for (std::size_t i = 0; i < results.capacity(); ++i) {
        results.push_back(product_t{.name = gen_next_str(), .quantity = gen_size()});
    }

    return results;
}

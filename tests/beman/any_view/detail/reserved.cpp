// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "reserved.hpp"

#include <algorithm>

auto reserved::database::get_products(query_t query) const -> names_t {
    const auto capacity = std::ranges::count_if(
        products, [=](const product_t& product) -> bool { return product.quantity >= query.min_quantity; });

    names_t results;
    results.reserve(capacity);

    for (const auto& product : products) {
        if (product.quantity >= query.min_quantity) {
            results.emplace_back(product.name);
        }
    }

    return results;
}

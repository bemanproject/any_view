// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "eager.hpp"

auto eager::database::get_products(query_t query) const -> names_t {
    names_t results;

    for (const auto& product : products) {
        if (product.quantity >= query.min_quantity) {
            results.push_back(product.name);
        }
    }

    return results;
}

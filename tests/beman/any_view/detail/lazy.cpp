// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "lazy.hpp"

auto lazy::database::get_products(query_t query) const -> names_t {
    return products | std::views::filter([=](const product_t& product) -> bool {
               return product.quantity >= query.min_quantity;
           }) |
           std::views::transform(&product_t::name);
}

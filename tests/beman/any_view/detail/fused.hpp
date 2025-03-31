// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "product.hpp"

#include <beman/any_view/any_view.hpp>

#include <vector>

namespace fused {

using names_t = beman::any_view::any_view<const std::string, beman::any_view::any_view_options::forward>;

struct database {
    std::vector<product_t> products;

    auto get_products(query_t) const -> names_t;
};

} // namespace fused

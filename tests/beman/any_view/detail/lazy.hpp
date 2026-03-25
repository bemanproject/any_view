// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "product.hpp"

#include <beman/any_view/any_view.hpp>

#include <vector>

namespace lazy {

using names_t = beman::any_view::any_view<const std::string>;

struct database {
    std::vector<product_t> products;

    names_t get_products(query_t) const;
};

} // namespace lazy

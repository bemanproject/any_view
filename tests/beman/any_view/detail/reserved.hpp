// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "product.hpp"

#include <vector>

namespace reserved {

using names_t = std::vector<std::string_view>;

struct database {
    std::vector<product_t> products;

    auto get_products(query_t) const -> names_t;
};

} // namespace reserved

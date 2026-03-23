// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "product.hpp"

#include <vector>

namespace reserved {

using names_t = std::vector<std::string_view>;

struct database {
    std::vector<product_t> products;

    names_t get_products(query_t) const;
};

} // namespace reserved

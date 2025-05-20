// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <string>

struct product_t {
    std::string name;
    std::size_t quantity;
};

struct query_t {
    std::size_t min_quantity;
};

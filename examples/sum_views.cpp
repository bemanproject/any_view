// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <array>
#include <iostream>
#include <ranges>
#include <vector>

namespace bav = beman::any_view;
using opt = bav::any_view_options;

auto sum(bav::any_view<const int> v1, bav::any_view<const int> v2) {
    auto result = 0;

    for (auto elem : v1) {
        result = result + elem;
    }

    for (auto elem : v2) {
        result = result + elem;
    }

    return result;
}

int main() {
    auto res = sum(std::vector{1, 2}, std::array<int, 2>{3, 4});
    std::cout << res << "\n";
    return 0;
}

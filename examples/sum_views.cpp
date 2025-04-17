// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <iostream>
#include <ranges>
#include <vector>

namespace bav = beman::any_view;
using opt = bav::any_view_options;

// Type alias for a generic input-range-like view over elements of type T.
// This wraps `bav::any_view` to abstract over different underlying range types
template <class T>
using proxy_any_view = bav::any_view<T, opt::input, T>;

// Computes the sum of all integers in a view of views of integers.
constexpr auto sum(proxy_any_view<proxy_any_view<int>> views) {
    auto result = 0;

    for (auto view : views) {
        for (const auto value : view) {
            result += value;
        }
    }

    return result;
}

int main() {
    auto res = sum(std::vector{std::vector{1, 2}, std::vector{3, 4}});
    std::cout << res << "\n";
    return 0;
}

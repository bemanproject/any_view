// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <ranges>

namespace bav = beman::any_view;

int main() {
    std::istringstream         in{"1 2 3 4"};
    bav::any_view<const int>   view{std::views::istream<int>(in)};
    std::ostream_iterator<int> out{std::cout, ", "};

    std::ranges::copy(view, out);
    std::cout << std::endl;

    return 0;
}

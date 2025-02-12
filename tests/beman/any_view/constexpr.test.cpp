// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

#include <algorithm>

using beman::any_view::any_view;
using enum beman::any_view::any_view_options;

template <class ValueT>
using proxy_any_view = any_view<ValueT, input, ValueT>;

constexpr auto sum(proxy_any_view<proxy_any_view<int>> views) {
    auto result = 0;

    for (auto view : views) {
        for (const auto value : view) {
            result += value;
        }
    }

    return result;
}

TEST(ConstexprTest, sum_vector_of_vector) {
#ifndef _MSC_VER
    // ICE on MSVC
    static_assert(10 == sum(std::vector{std::vector{1, 2}, std::vector{3, 4}}));
#endif
    EXPECT_EQ(10, sum(std::vector{std::vector{1, 2}, std::vector{3, 4}}));
}

constexpr auto iota(int n) { return std::views::iota(1) | std::views::take(n); };

TEST(ConstexprTest, sum_transform_view_of_iota_view) {
    constexpr auto view = iota(5) | std::views::transform(iota);

#ifndef _MSC_VER
    // ICE on MSVC
    static_assert(35 == sum(view));
#endif
    EXPECT_EQ(35, sum(view));
}

TEST(ConstexprTest, sum_vector_of_iota_view) {
#ifndef _MSC_VER
    // ICE on MSVC
    static_assert(22 == sum(std::vector{iota(1), iota(3), iota(5)}));
#endif
    EXPECT_EQ(22, sum(std::vector{iota(1), iota(3), iota(5)}));
}

constexpr auto sort(any_view<int, random_access> view) {
    std::ranges::sort(view);
    return std::ranges::is_sorted(view);
}

TEST(ConstexprTest, sort_vector) {
#ifdef __clang__
    // ICE on GCC and MSVC
    static_assert(sort(std::vector{6, 8, 7, 5, 3, 0, 9}));
#endif
    EXPECT_TRUE(sort(std::vector{6, 8, 7, 5, 3, 0, 9}));
}

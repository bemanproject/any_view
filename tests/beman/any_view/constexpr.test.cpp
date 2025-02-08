// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

#include <vector>

using beman::any_view::any_view;

#if BEMAN_ANY_VIEW_USE_ENUM()
using enum beman::any_view::any_view_options;

template <class ValueT>
using proxy_any_view = any_view<ValueT,
                                input
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
                                    | move_only
#endif
                                ,
                                ValueT>;
#elif BEMAN_ANY_VIEW_USE_TRAITS()
template <class ValueT>
struct proxy_traits {
    using reference_type = ValueT;
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    static constexpr bool move_only = true;
#endif
};

template <class ValueT>
using proxy_any_view = any_view<ValueT, proxy_traits<ValueT>>;
#elif BEMAN_ANY_VIEW_USE_NAMED()
using beman::any_view::type;

template <class ValueT>
using proxy_any_view = any_view<ValueT,
                                {
                                    .reference_type = type<ValueT>,
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
                                    .move_only = true,
#endif
                                }>;
#endif

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
    static_assert(15 == sum(std::vector{std::vector{1, 2}, std::vector{3, 4}, std::vector{5}}));
    EXPECT_EQ(15, sum(std::vector{std::vector{1, 2}, std::vector{3, 4}, std::vector{5}}));
}

TEST(ConstexprTest, sum_transform_view_of_iota) {
    constexpr auto iota = [](int n) { return std::views::iota(1) | std::views::take(n); };
    constexpr auto view = iota(5) | std::views::transform(iota);

    static_assert(35 == sum(view));
    EXPECT_EQ(35, sum(view));
}

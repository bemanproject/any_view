// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "detail/tracing_iterator.hpp"

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

#include <string>

using beman::any_view::any_view;
using enum beman::any_view::any_view_options;

template <class RefT>
using any_random_access_view = any_view<RefT, random_access, RefT>;

TEST(IteratorTest, random_access_operators) {
    auto transformed = std::views::iota(0) | std::views::transform([](int n) { return "val_" + std::to_string(n); });
    auto view        = any_random_access_view<std::string>{transformed};

    auto it1 = view.begin();
    auto it2 = view.begin();
    it2 += 5;

    EXPECT_EQ(*it1, "val_0");
    EXPECT_EQ(*it2, "val_5");

    ++it1;
    EXPECT_EQ(*it1, "val_1");

    it1 += 3;
    EXPECT_EQ(*it1, "val_4");

    EXPECT_EQ(it2 - it1, 1);
    EXPECT_TRUE(it1 < it2);

    --it2;
    EXPECT_EQ(*it2, "val_4");
    EXPECT_TRUE(it1 == it2);
}

TEST(IteratorTest, random_access_optimization) {
    auto transformed = std::views::iota(0) | std::views::transform([](int n) { return "val_" + std::to_string(n); });
    auto offset      = std::ptrdiff_t{};
    auto begin       = tracing_iterator{transformed.begin(), &offset};
    auto end         = transformed.end();
    auto subrange    = std::ranges::subrange{begin, end};
    auto view        = any_random_access_view<std::string>{subrange};
    auto it          = std::ranges::begin(view);

    // incrementing optimized random access iterator does not increment underlying iterator
    ++it;
    EXPECT_EQ(offset, 0);

    // dereferencing it advances underlying iterator
    EXPECT_EQ(*it, "val_1");
    EXPECT_EQ(offset, 1);

    // advancing optimized random access iterator does not advance underlying iterator
    offset = 0;
    it += 2;
    EXPECT_EQ(offset, 0);

    // dereferencing it advances the underlying iterator by the total offset from begin
    EXPECT_EQ(*it, "val_3");
    EXPECT_EQ(offset, 3);
}

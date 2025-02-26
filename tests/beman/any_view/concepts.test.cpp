// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

using beman::any_view::any_view;
using enum beman::any_view::any_view_options;

TEST(ConceptsTest, iterator_concept) {
    static_assert(std::ranges::input_range<any_view<int>>);
    static_assert(not std::ranges::forward_range<any_view<int>>);
    static_assert(std::ranges::input_range<any_view<int, input>>);
    static_assert(not std::ranges::forward_range<any_view<int, input>>);
    static_assert(std::ranges::forward_range<any_view<int, forward>>);
    static_assert(not std::ranges::bidirectional_range<any_view<int, forward>>);
    static_assert(std::ranges::bidirectional_range<any_view<int, bidirectional>>);
    static_assert(not std::ranges::random_access_range<any_view<int, bidirectional>>);
    static_assert(std::ranges::random_access_range<any_view<int, random_access>>);
    static_assert(not std::ranges::contiguous_range<any_view<int, random_access>>);
    static_assert(std::ranges::contiguous_range<any_view<int, contiguous>>);

    static_assert(std::ranges::input_range<any_view<int, input | sized>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | sized>>);
    static_assert(std::ranges::input_range<any_view<int, input | borrowed>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | borrowed>>);
    static_assert(std::ranges::input_range<any_view<int, input | borrowed>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | borrowed>>);
    static_assert(std::ranges::input_range<any_view<int, input | copyable>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | copyable>>);
}

TEST(ConceptsTest, sized_concept) {
    static_assert(not std::ranges::sized_range<any_view<int>>);
    static_assert(std::ranges::sized_range<any_view<int, input | sized>>);

    static_assert(not std::ranges::sized_range<any_view<int, input>>);
    static_assert(not std::ranges::sized_range<any_view<int, forward>>);
    static_assert(not std::ranges::sized_range<any_view<int, bidirectional>>);
    static_assert(not std::ranges::sized_range<any_view<int, random_access>>);
    static_assert(not std::ranges::sized_range<any_view<int, contiguous>>);
    static_assert(not std::ranges::sized_range<any_view<int, input | borrowed>>);
    static_assert(not std::ranges::sized_range<any_view<int, input | copyable>>);
}

TEST(ConceptsTest, borrowed_concept) {
    static_assert(not std::ranges::borrowed_range<any_view<int>>);
    static_assert(std::ranges::borrowed_range<any_view<int, input | borrowed>>);

    static_assert(not std::ranges::borrowed_range<any_view<int, input>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, forward>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, bidirectional>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, random_access>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, contiguous>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, input | sized>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, input | copyable>>);
}

TEST(ConceptsTest, copyable_concept) {
    static_assert(not std::copyable<any_view<int>>);
    static_assert(std::copyable<any_view<int, input | copyable>>);

    static_assert(not std::copyable<any_view<int, input>>);
    static_assert(not std::copyable<any_view<int, forward>>);
    static_assert(not std::copyable<any_view<int, bidirectional>>);
    static_assert(not std::copyable<any_view<int, random_access>>);
    static_assert(not std::copyable<any_view<int, contiguous>>);
    static_assert(not std::copyable<any_view<int, input | sized>>);
    static_assert(not std::copyable<any_view<int, input | borrowed>>);
}

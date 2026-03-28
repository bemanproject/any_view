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

    static_assert(std::ranges::input_range<any_view<int, input | approximately_sized>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | approximately_sized>>);
    static_assert(std::ranges::input_range<any_view<int, input | sized>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | sized>>);
    static_assert(std::ranges::input_range<any_view<int, input | borrowed>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | borrowed>>);
    static_assert(std::ranges::input_range<any_view<int, input | copyable>>);
    static_assert(not std::ranges::forward_range<any_view<int, input | copyable>>);
}

TEST(ConceptsTest, approximately_sized_concept) {
    static_assert(not beman::any_view::approximately_sized_range<any_view<int>>);
    static_assert(beman::any_view::approximately_sized_range<any_view<int, input | approximately_sized>>);
    static_assert(beman::any_view::approximately_sized_range<any_view<int, input | sized>>);

    static_assert(not beman::any_view::approximately_sized_range<any_view<int, input>>);
    static_assert(not beman::any_view::approximately_sized_range<any_view<int, forward>>);
    static_assert(not beman::any_view::approximately_sized_range<any_view<int, bidirectional>>);
    static_assert(not beman::any_view::approximately_sized_range<any_view<int, random_access>>);
    static_assert(not beman::any_view::approximately_sized_range<any_view<int, contiguous>>);
    static_assert(not beman::any_view::approximately_sized_range<any_view<int, input | borrowed>>);
    static_assert(not beman::any_view::approximately_sized_range<any_view<int, input | copyable>>);
}

TEST(ConceptsTest, sized_concept) {
    static_assert(not std::ranges::sized_range<any_view<int>>);
    static_assert(std::ranges::sized_range<any_view<int, input | sized>>);

    static_assert(not std::ranges::sized_range<any_view<int, input>>);
    static_assert(not std::ranges::sized_range<any_view<int, forward>>);
    static_assert(not std::ranges::sized_range<any_view<int, bidirectional>>);
    static_assert(not std::ranges::sized_range<any_view<int, random_access>>);
    static_assert(not std::ranges::sized_range<any_view<int, contiguous>>);
    static_assert(not std::ranges::sized_range<any_view<int, input | approximately_sized>>);
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
    static_assert(not std::ranges::borrowed_range<any_view<int, input | approximately_sized>>);
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
    static_assert(not std::copyable<any_view<int, input | approximately_sized>>);
    static_assert(not std::copyable<any_view<int, input | sized>>);
    static_assert(not std::copyable<any_view<int, input | borrowed>>);
}

TEST(ConceptsTest, common_view_concept) {
    // These error through R6 because constraining constructor with viewable_range or copyable caused it to depend on
    // itself. Important because view_interface member functions cannot be used if view constraint errors.
    static_assert(std::ranges::view<std::ranges::common_view<any_view<int, forward>>>);
    static_assert(not std::copyable<std::ranges::common_view<any_view<int, forward>>>);
    static_assert(std::copyable<std::ranges::common_view<any_view<int, forward | copyable>>>);
}

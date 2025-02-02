// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include "options.hpp"

#include <gtest/gtest.h>

using beman::any_view::any_view;

TEST(ConceptsTest, iterator_concept) {
    static_assert(std::ranges::input_range<any_view<int>>);
    static_assert(not std::ranges::forward_range<any_view<int>>);
    static_assert(std::ranges::input_range<any_view<int, input_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, input_options>>);
    static_assert(std::ranges::forward_range<any_view<int, forward_options>>);
    static_assert(not std::ranges::bidirectional_range<any_view<int, forward_options>>);
    static_assert(std::ranges::bidirectional_range<any_view<int, bidirectional_options>>);
    static_assert(not std::ranges::random_access_range<any_view<int, bidirectional_options>>);
    static_assert(std::ranges::random_access_range<any_view<int, random_access_options>>);
    static_assert(not std::ranges::contiguous_range<any_view<int, random_access_options>>);
    static_assert(std::ranges::contiguous_range<any_view<int, contiguous_options>>);

    static_assert(std::ranges::input_range<any_view<int, sized_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, sized_options>>);
    static_assert(std::ranges::input_range<any_view<int, borrowed_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, borrowed_options>>);
    static_assert(std::ranges::input_range<any_view<int, borrowed_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, borrowed_options>>);
    static_assert(std::ranges::input_range<any_view<int, copyable_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, copyable_options>>);
    static_assert(std::ranges::input_range<any_view<int, move_only_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, move_only_options>>);
    static_assert(std::ranges::input_range<any_view<int, simple_options>>);
    static_assert(not std::ranges::forward_range<any_view<int, simple_options>>);
}

TEST(ConceptsTest, sized_concept) {
    static_assert(not std::ranges::sized_range<any_view<int>>);
    static_assert(std::ranges::sized_range<any_view<int, sized_options>>);

    static_assert(not std::ranges::sized_range<any_view<int, input_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, forward_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, bidirectional_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, random_access_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, contiguous_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, borrowed_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, copyable_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, move_only_options>>);
    static_assert(not std::ranges::sized_range<any_view<int, simple_options>>);
}

TEST(ConceptsTest, borrowed_concept) {
    static_assert(not std::ranges::borrowed_range<any_view<int>>);
    static_assert(std::ranges::borrowed_range<any_view<int, borrowed_options>>);

    static_assert(not std::ranges::borrowed_range<any_view<int, input_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, forward_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, bidirectional_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, random_access_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, contiguous_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, sized_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, copyable_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, move_only_options>>);
    static_assert(not std::ranges::borrowed_range<any_view<int, simple_options>>);
}

TEST(ConceptsTest, copyable_concept) {
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    static_assert(not std::copyable<any_view<int>>);
    static_assert(not std::copyable<any_view<int, move_only_options>>);
    static_assert(std::copyable<any_view<int, copyable_options>>);
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    static_assert(std::copyable<any_view<int>>);
    static_assert(std::copyable<any_view<int, copyable_options>>);
    static_assert(not std::copyable<any_view<int, move_only_options>>);
#endif

    // the outcome of these options would normally depend on copyable/move_only configuration, but for testing
    // purposes, other options opt-in to move-only behavior for both configured modes
    static_assert(not std::copyable<any_view<int, input_options>>);
    static_assert(not std::copyable<any_view<int, forward_options>>);
    static_assert(not std::copyable<any_view<int, bidirectional_options>>);
    static_assert(not std::copyable<any_view<int, random_access_options>>);
    static_assert(not std::copyable<any_view<int, contiguous_options>>);
    static_assert(not std::copyable<any_view<int, sized_options>>);
    static_assert(not std::copyable<any_view<int, borrowed_options>>);
    static_assert(not std::copyable<any_view<int, simple_options>>);
}

// https://en.cppreference.com/w/cpp/ranges#simple-view
template <class RangeT>
concept simple_view = std::ranges::view<RangeT> and std::ranges::range<const RangeT> and
                      std::same_as<std::ranges::iterator_t<RangeT>, std::ranges::iterator_t<const RangeT>> and
                      std::same_as<std::ranges::sentinel_t<RangeT>, std::ranges::sentinel_t<const RangeT>>;

TEST(ConceptsTest, simple_concept) {
    // to clarify, any_view is always a view. This test demonstrates when it is specifically a simple_view
    static_assert(std::ranges::view<any_view<int>>);

    static_assert(not simple_view<any_view<int>>);
    static_assert(simple_view<any_view<int, simple_options>>);

    static_assert(not simple_view<any_view<int, input_options>>);
    static_assert(not simple_view<any_view<int, forward_options>>);
    static_assert(not simple_view<any_view<int, bidirectional_options>>);
    static_assert(not simple_view<any_view<int, random_access_options>>);
    static_assert(not simple_view<any_view<int, contiguous_options>>);
    static_assert(not simple_view<any_view<int, sized_options>>);
    static_assert(not simple_view<any_view<int, borrowed_options>>);
    static_assert(not simple_view<any_view<int, copyable_options>>);
    static_assert(not simple_view<any_view<int, move_only_options>>);
}

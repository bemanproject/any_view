// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

#if BEMAN_ANY_VIEW_USE_TRAITS()

template <class IterConceptT>
struct options {
    using iterator_concept = IterConceptT;
};

#elif BEMAN_ANY_VIEW_USE_NAMED()

template <class IterConceptT>
using options = beman::any_view::any_view_options options{
    .reference_type   = beman::any_view::type<int&>,
    .iterator_concept = beman::any_view::type<IterConceptT>,
};

#endif

TEST(ConceptsTest, iterator_concept) {
    using beman::any_view::any_view;

#if BEMAN_ANY_VIEW_USE_FLAGS()
    using options = beman::any_view::any_view_options;

    static_assert(std::ranges::input_range<any_view<int, options::input>>);
    static_assert(not std::ranges::forward_range<any_view<int, options::input>>);
    static_assert(std::ranges::forward_range<any_view<int, options::forward>>);
    static_assert(not std::ranges::bidirectional_range<any_view<int, options::forward>>);
    static_assert(std::ranges::bidirectional_range<any_view<int, options::bidirectional>>);
    static_assert(not std::ranges::random_access_range<any_view<int, options::bidirectional>>);
    static_assert(std::ranges::random_access_range<any_view<int, options::random_access>>);
    static_assert(not std::ranges::contiguous_range<any_view<int, options::random_access>>);
    static_assert(std::ranges::contiguous_range<any_view<int, options::contiguous>>);
#else
    static_assert(std::ranges::input_range<any_view<int, options<std::input_iterator_tag>>>);
    static_assert(not std::ranges::forward_range<any_view<int, options<std::input_iterator_tag>>>);
    static_assert(std::ranges::forward_range<any_view<int, options<std::forward_iterator_tag>>>);
    static_assert(not std::ranges::bidirectional_range<any_view<int, options<std::forward_iterator_tag>>>);
    static_assert(std::ranges::bidirectional_range<any_view<int, options<std::bidirectional_iterator_tag>>>);
    static_assert(not std::ranges::random_access_range<any_view<int, options<std::bidirectional_iterator_tag>>>);
    static_assert(std::ranges::random_access_range<any_view<int, options<std::random_access_iterator_tag>>>);
    static_assert(not std::ranges::contiguous_range<any_view<int, options<std::random_access_iterator_tag>>>);
    static_assert(std::ranges::contiguous_range<any_view<int, options<std::contiguous_iterator_tag>>>);
#endif
}

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

using beman::any_view::any_view;

#if BEMAN_ANY_VIEW_USE_FLAGS()

using options = beman::any_view::any_view_options;

inline constexpr auto input_options         = options::input;
inline constexpr auto forward_options       = options::forward;
inline constexpr auto bidirectional_options = options::bidirectional;
inline constexpr auto random_access_options = options::random_access;
inline constexpr auto contiguous_options    = options::contiguous;

inline constexpr auto sized_options = options::input | options::sized;

inline constexpr auto borrowed_options = options::input | options::borrowed;

inline constexpr auto BEMAN_ANY_VIEW_OPTION_(options) = options::input | options::BEMAN_ANY_VIEW_OPTION();

inline constexpr auto simple_options = options::input | options::simple;

#elif BEMAN_ANY_VIEW_USE_TRAITS()

template <class IterConceptT>
struct iterator_options {
    using iterator_concept = IterConceptT;
};

using input_options         = iterator_options<std::input_iterator_tag>;
using forward_options       = iterator_options<std::forward_iterator_tag>;
using bidirectional_options = iterator_options<std::bidirectional_iterator_tag>;
using random_access_options = iterator_options<std::random_access_iterator_tag>;
using contiguous_options    = iterator_options<std::contiguous_iterator_tag>;

struct sized_options {
    static constexpr bool sized = true;
};

struct borrowed_options {
    static constexpr bool borrowed = true;
};

struct BEMAN_ANY_VIEW_OPTION_(options) {
    static constexpr bool BEMAN_ANY_VIEW_OPTION() = true;
};

struct simple_options {
    static constexpr bool simple = true;
};

#elif BEMAN_ANY_VIEW_USE_NAMED()

using beman::any_view::any_view_options;
using beman::any_view::type;

inline constexpr auto default_reference_type = type<int&>;

template <class IterConceptT>
inline constexpr any_view_options iterator_options{
    .reference_type   = default_reference_type,
    .iterator_concept = type<IterConceptT>,
};

inline constexpr auto input_options         = iterator_options<std::input_iterator_tag>;
inline constexpr auto forward_options       = iterator_options<std::forward_iterator_tag>;
inline constexpr auto bidirectional_options = iterator_options<std::bidirectional_iterator_tag>;
inline constexpr auto random_access_options = iterator_options<std::random_access_iterator_tag>;
inline constexpr auto contiguous_options    = iterator_options<std::contiguous_iterator_tag>;

inline constexpr any_view_options sized_options{
    .reference_type = default_reference_type,
    .sized          = true,
};

inline constexpr any_view_options borrowed_options{
    .reference_type = default_reference_type,
    .borrowed       = true,
};

inline constexpr any_view_options BEMAN_ANY_VIEW_OPTION_(options){
    .reference_type          = default_reference_type,
    .BEMAN_ANY_VIEW_OPTION() = true,
};

inline constexpr any_view_options simple_options{
    .reference_type = default_reference_type,
    .simple         = true,
};

#endif

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

    static_assert(std::ranges::input_range<any_view<int, BEMAN_ANY_VIEW_OPTION_(options)>>);
    static_assert(not std::ranges::forward_range<any_view<int, BEMAN_ANY_VIEW_OPTION_(options)>>);

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

    static_assert(not std::ranges::sized_range<any_view<int, BEMAN_ANY_VIEW_OPTION_(options)>>);

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

    static_assert(not std::ranges::borrowed_range<any_view<int, BEMAN_ANY_VIEW_OPTION_(options)>>);

    static_assert(not std::ranges::borrowed_range<any_view<int, simple_options>>);
}

TEST(ConceptsTest, copyable_concept) {
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    static_assert(not std::copyable<any_view<int>>);
    static_assert(std::copyable<any_view<int, copyable_options>>);

    static_assert(not std::copyable<any_view<int, input_options>>);
    static_assert(not std::copyable<any_view<int, forward_options>>);
    static_assert(not std::copyable<any_view<int, bidirectional_options>>);
    static_assert(not std::copyable<any_view<int, random_access_options>>);
    static_assert(not std::copyable<any_view<int, contiguous_options>>);

    static_assert(not std::copyable<any_view<int, sized_options>>);

    static_assert(not std::copyable<any_view<int, borrowed_options>>);

    static_assert(not std::copyable<any_view<int, simple_options>>);
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    static_assert(std::copyable<any_view<int>>);
    static_assert(not std::copyable<any_view<int, move_only_options>>);

    static_assert(std::copyable<any_view<int, input_options>>);
    static_assert(std::copyable<any_view<int, forward_options>>);
    static_assert(std::copyable<any_view<int, bidirectional_options>>);
    static_assert(std::copyable<any_view<int, random_access_options>>);
    static_assert(std::copyable<any_view<int, contiguous_options>>);

    static_assert(std::copyable<any_view<int, sized_options>>);

    static_assert(std::copyable<any_view<int, borrowed_options>>);

    static_assert(std::copyable<any_view<int, simple_options>>);
#endif
}

// https://en.cppreference.com/w/cpp/ranges#simple-view
template <class RangeT>
concept simple_view = std::ranges::view<RangeT> and std::ranges::range<const RangeT> and
                      std::same_as<std::ranges::iterator_t<RangeT>, std::ranges::iterator_t<const RangeT>> and
                      std::same_as<std::ranges::sentinel_t<RangeT>, std::ranges::sentinel_t<const RangeT>>;

TEST(ConceptsTest, simple_concept) {
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

    static_assert(not simple_view<any_view<int, BEMAN_ANY_VIEW_OPTION_(options)>>);
}

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

#include <utility>

using beman::any_view::any_view;
using value  = std::pair<int, float>;
using lvalue = std::pair<int&, float&>;
using rvalue = std::pair<int&&, float&&>;

TEST(TypeTraitsTest, value_type) {
    using std::ranges::range_value_t;

    static_assert(std::same_as<range_value_t<any_view<value>>, value>);
    static_assert(std::same_as<range_value_t<any_view<const value>>, value>);
}

struct value_traits {
    using reference_type = value;
};

struct lvalue_traits {
    using reference_type = lvalue;
};

TEST(TypeTraitsTest, reference_type) {
    using std::ranges::range_reference_t;

    static_assert(std::same_as<range_reference_t<any_view<value>>, value&>);
    static_assert(std::same_as<range_reference_t<any_view<const value>>, const value&>);

#if BEMAN_ANY_VIEW_USE_FLAGS()
    using enum beman::any_view::any_view_options;

    static_assert(std::same_as<range_reference_t<any_view<value, input, value>>, value>);
    static_assert(std::same_as<range_reference_t<any_view<value, input, lvalue>>, lvalue>);
#elif BEMAN_ANY_VIEW_USE_TRAITS()
    static_assert(std::same_as<range_reference_t<any_view<value, value_traits>>, value>);
    static_assert(std::same_as<range_reference_t<any_view<value, lvalue_traits>>, lvalue>);
#elif BEMAN_ANY_VIEW_USE_NAMED()
    using beman::any_view::type;

    static_assert(std::same_as<range_reference_t<any_view<value, {.reference_type = type<value>}>>, value>);
    static_assert(std::same_as<range_reference_t<any_view<lvalue, {.reference_type = type<lvalue>}>>, lvalue>);
#endif
}

struct rvalue_traits {
    using rvalue_reference_type = rvalue;
};

TEST(TypeTraitsTest, rvalue_reference_type) {
    using std::ranges::range_rvalue_reference_t;

    static_assert(std::same_as<range_rvalue_reference_t<any_view<value>>, value&&>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<const value>>, const value&&>);

#if BEMAN_ANY_VIEW_USE_FLAGS()
    using enum beman::any_view::any_view_options;

    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, input, value>>, value>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, input, lvalue>>, lvalue>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, input, lvalue, rvalue>>, rvalue>);
#elif BEMAN_ANY_VIEW_USE_TRAITS()
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, value_traits>>, value>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, lvalue_traits>>, lvalue>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, rvalue_traits>>, rvalue>);
#elif BEMAN_ANY_VIEW_USE_NAMED()
    using beman::any_view::type;

    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, {.reference_type = type<value>}>>, value>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, {.reference_type = type<lvalue>}>>, lvalue>);
    static_assert(
        std::same_as<range_rvalue_reference_t<
                         any_view<value, {.reference_type = type<lvalue>, .rvalue_reference_type = type<rvalue>}>>,
                     rvalue>);
#endif
}

struct difference_traits {
    using difference_type = short;
};

TEST(TypeTraitsTest, difference_type) {
    using std::ranges::range_difference_t;

    static_assert(std::same_as<range_difference_t<any_view<value>>, std::ptrdiff_t>);

#if BEMAN_ANY_VIEW_USE_FLAGS()
    using enum beman::any_view::any_view_options;

    static_assert(std::same_as<range_difference_t<any_view<value, input, lvalue, rvalue, short>>, short>);
#elif BEMAN_ANY_VIEW_USE_TRAITS()
    static_assert(std::same_as<range_difference_t<any_view<value, difference_traits>>, short>);
#elif BEMAN_ANY_VIEW_USE_NAMED()
    using beman::any_view::type;

    static_assert(
        std::same_as<
            range_difference_t<any_view<value, {.reference_type = type<lvalue>, .difference_type = type<short>}>>,
            short>);
#endif
}

struct sized_traits {
    static constexpr bool sized = true;
};

struct sized_difference_traits : sized_traits {
    using difference_type = short;
};

TEST(TypeTraitsTest, size_type) {
    using std::ranges::range_size_t;

#if BEMAN_ANY_VIEW_USE_FLAGS()
    using enum beman::any_view::any_view_options;

    static_assert(std::same_as<range_size_t<any_view<value, input | sized>>, std::size_t>);
    static_assert(std::same_as<range_size_t<any_view<value, input | sized, lvalue, rvalue, short>>, unsigned short>);
#elif BEMAN_ANY_VIEW_USE_TRAITS()
    static_assert(std::same_as<range_size_t<any_view<value, sized_traits>>, std::size_t>);
    static_assert(std::same_as<range_size_t<any_view<value, sized_difference_traits>>, unsigned short>);
#elif BEMAN_ANY_VIEW_USE_NAMED()
    using beman::any_view::type;

    static_assert(
        std::same_as<range_size_t<any_view<value, {.reference_type = type<lvalue>, .sized = true}>>, std::size_t>);
    static_assert(
        std::same_as<
            range_size_t<
                any_view<value, {.reference_type = type<lvalue>, .sized = true, .difference_type = type<short>}>>,
            unsigned short>);
#endif
}

struct borrowed_traits {
    static constexpr bool borrowed = true;
};

TEST(TypeTraitsTest, borrowed_iterator_type) {
    using std::ranges::borrowed_iterator_t;
    using std::ranges::iterator_t;

    static_assert(std::same_as<borrowed_iterator_t<any_view<value>>, std::ranges::dangling>);

#if BEMAN_ANY_VIEW_USE_FLAGS()
    using enum beman::any_view::any_view_options;

    static_assert(std::same_as<borrowed_iterator_t<any_view<value, input | borrowed>>,
                               iterator_t<any_view<value, input | borrowed>>>);
#elif BEMAN_ANY_VIEW_USE_TRAITS()
    static_assert(std::same_as<borrowed_iterator_t<any_view<value, borrowed_traits>>,
                               iterator_t<any_view<value, borrowed_traits>>>);
#elif BEMAN_ANY_VIEW_USE_NAMED()
    using beman::any_view::type;

    static_assert(
        std::same_as<borrowed_iterator_t<any_view<value, {.reference_type = type<lvalue>, .borrowed = true}>>,
                     iterator_t<any_view<value, {.reference_type = type<lvalue>, .borrowed = true}>>>);
#endif
}

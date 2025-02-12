// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

using beman::any_view::any_view;
using enum beman::any_view::any_view_options;

using value  = std::pair<int, float>;
using lvalue = std::pair<int&, float&>;
using rvalue = std::pair<int&&, float&&>;

TEST(TypeTraitsTest, value_type) {
    using std::ranges::range_value_t;

    static_assert(std::same_as<range_value_t<any_view<value>>, value>);
    static_assert(std::same_as<range_value_t<any_view<const value>>, value>);
}

TEST(TypeTraitsTest, reference_type) {
    using std::ranges::range_reference_t;

    static_assert(std::same_as<range_reference_t<any_view<value>>, value&>);
    static_assert(std::same_as<range_reference_t<any_view<const value>>, const value&>);

    static_assert(std::same_as<range_reference_t<any_view<value, input, value>>, value>);
    static_assert(std::same_as<range_reference_t<any_view<value, input, lvalue>>, lvalue>);
}

TEST(TypeTraitsTest, rvalue_reference_type) {
    using std::ranges::range_rvalue_reference_t;

    static_assert(std::same_as<range_rvalue_reference_t<any_view<value>>, value&&>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<const value>>, const value&&>);

    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, input, value>>, value>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, input, lvalue>>, lvalue>);
    static_assert(std::same_as<range_rvalue_reference_t<any_view<value, input, lvalue, rvalue>>, rvalue>);
}

TEST(TypeTraitsTest, difference_type) {
    using std::ranges::range_difference_t;

    static_assert(std::same_as<range_difference_t<any_view<value>>, std::ptrdiff_t>);

    static_assert(std::same_as<range_difference_t<any_view<value, input, lvalue, rvalue, short>>, short>);
}

TEST(TypeTraitsTest, size_type) {
    using std::ranges::range_size_t;

    static_assert(std::same_as<range_size_t<any_view<value, input | sized>>, std::size_t>);
    static_assert(std::same_as<range_size_t<any_view<value, input | sized, value&, value&&, short>>, unsigned short>);
}

TEST(TypeTraitsTest, borrowed_iterator_type) {
    using std::ranges::borrowed_iterator_t;
    using std::ranges::iterator_t;

    static_assert(std::same_as<borrowed_iterator_t<any_view<value>>, std::ranges::dangling>);

    static_assert(std::same_as<borrowed_iterator_t<any_view<value, input | borrowed>>,
                               iterator_t<any_view<value, input | borrowed>>>);
}

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include <gtest/gtest.h>

#include <deque>
#include <forward_list>
#include <list>

using beman::any_view::any_view;
using enum beman::any_view::any_view_options;

TEST(SfinaeTest, istream_view) {
    static_assert(std::constructible_from<any_view<int, input>, std::ranges::istream_view<int>>);
    static_assert(not std::constructible_from<any_view<int, forward>, std::ranges::istream_view<int>>);
    static_assert(std::constructible_from<any_view<int, input | copyable>, std::ranges::istream_view<int>>);
    static_assert(not std::constructible_from<any_view<int, input | sized>, std::ranges::istream_view<int>>);
    static_assert(not std::constructible_from<any_view<int, input | borrowed>, std::ranges::istream_view<int>>);

    // This may be surprising, but any_view makes a copy of an lvalue reference to view because of std::views::all
    // It limits some use-cases, but it also prevents some potential dangling when returning any_view from a function
    static_assert(not std::constructible_from<any_view<int, input | borrowed>, std::ranges::istream_view<int>&>);
}

TEST(SfinaeTest, forward_list) {
    static_assert(std::constructible_from<any_view<int, forward>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, bidirectional>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, input | sized>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, input | borrowed>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, input | copyable>, std::forward_list<int>>);

    static_assert(std::constructible_from<any_view<int, input | borrowed>, std::forward_list<int>&>);
    static_assert(std::constructible_from<any_view<int, input | copyable>, std::forward_list<int>&>);
}

TEST(SfinaeTest, list) {
    static_assert(std::constructible_from<any_view<int, bidirectional>, std::list<int>>);
    static_assert(not std::constructible_from<any_view<int, random_access>, std::list<int>>);
    static_assert(std::constructible_from<any_view<int, input | sized>, std::list<int>>);
    static_assert(not std::constructible_from<any_view<int, input | borrowed>, std::list<int>>);
    static_assert(not std::constructible_from<any_view<int, input | copyable>, std::list<int>>);

    static_assert(std::constructible_from<any_view<int, input | borrowed>, std::list<int>&>);
    static_assert(std::constructible_from<any_view<int, input | copyable>, std::list<int>&>);
}

TEST(SfinaeTest, deque) {
    static_assert(std::constructible_from<any_view<int, random_access>, std::deque<int>>);
    static_assert(not std::constructible_from<any_view<int, contiguous>, std::deque<int>>);
    static_assert(std::constructible_from<any_view<int, input | sized>, std::deque<int>>);
    static_assert(not std::constructible_from<any_view<int, input | borrowed>, std::deque<int>>);
    static_assert(not std::constructible_from<any_view<int, input | copyable>, std::deque<int>>);

    static_assert(std::constructible_from<any_view<int, input | borrowed>, std::deque<int>&>);
    static_assert(std::constructible_from<any_view<int, input | copyable>, std::deque<int>&>);
}

TEST(SfinaeTest, vector) {
    static_assert(std::constructible_from<any_view<int, contiguous>, std::vector<int>>);
    static_assert(std::constructible_from<any_view<int, input | sized>, std::vector<int>>);
    static_assert(not std::constructible_from<any_view<int, input | borrowed>, std::vector<int>>);
    static_assert(not std::constructible_from<any_view<int, input | copyable>, std::vector<int>>);

    static_assert(std::constructible_from<any_view<int, input | borrowed>, std::vector<int>&>);
    static_assert(std::constructible_from<any_view<int, input | copyable>, std::vector<int>&>);
}

TEST(SfinaeTest, vector_of_bool) {
    static_assert(std::constructible_from<any_view<bool, random_access, bool>, std::vector<bool>>);
    static_assert(std::constructible_from<any_view<bool, input | sized, bool>, std::vector<bool>>);
    static_assert(not std::constructible_from<any_view<bool, input | borrowed, bool>, std::vector<bool>>);
    static_assert(not std::constructible_from<any_view<bool, input | copyable, bool>, std::vector<bool>>);

    static_assert(std::constructible_from<any_view<bool, input | borrowed, bool>, std::vector<bool>&>);
    static_assert(std::constructible_from<any_view<bool, input | copyable, bool>, std::vector<bool>&>);
}

TEST(SfinaeTest, span) {
    static_assert(std::constructible_from<any_view<int, contiguous>, std::span<int>>);
    static_assert(std::constructible_from<any_view<int, input | sized>, std::span<int>>);
    static_assert(std::constructible_from<any_view<int, input | borrowed>, std::span<int>>);
    static_assert(std::constructible_from<any_view<int, input | copyable>, std::span<int>>);
}

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/any_view/any_view.hpp>

#include "options.hpp"

#include <gtest/gtest.h>

#include <deque>
#include <forward_list>
#include <list>
#include <span>
#include <vector>

using beman::any_view::any_view;

TEST(SfinaeTest, istream_view) {
    static_assert(std::constructible_from<any_view<int, input_options>, std::ranges::istream_view<int>>);
    static_assert(not std::constructible_from<any_view<int, forward_options>, std::ranges::istream_view<int>>);
    static_assert(std::constructible_from<any_view<int, copyable_options>, std::ranges::istream_view<int>>);
    static_assert(not std::constructible_from<any_view<int, sized_options>, std::ranges::istream_view<int>>);
    static_assert(not std::constructible_from<any_view<int, borrowed_options>, std::ranges::istream_view<int>>);

    // This may be surprising, but any_view makes a copy of an lvalue reference to view because of std::views::all
    // It limits some use-cases, but it also prevents some potential dangling when returning any_view from a function
    static_assert(not std::constructible_from<any_view<int, borrowed_options>, std::ranges::istream_view<int>&>);
}

TEST(SfinaeTest, forward_list) {
    static_assert(std::constructible_from<any_view<int, forward_options>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, bidirectional_options>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, sized_options>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, borrowed_options>, std::forward_list<int>>);
    static_assert(not std::constructible_from<any_view<int, copyable_options>, std::forward_list<int>>);

    static_assert(std::constructible_from<any_view<int, borrowed_options>, std::forward_list<int>&>);
    static_assert(std::constructible_from<any_view<int, copyable_options>, std::forward_list<int>&>);
}

TEST(SfinaeTest, list) {
    static_assert(std::constructible_from<any_view<int, bidirectional_options>, std::list<int>>);
    static_assert(not std::constructible_from<any_view<int, random_access_options>, std::list<int>>);
    static_assert(std::constructible_from<any_view<int, sized_options>, std::list<int>>);
    static_assert(not std::constructible_from<any_view<int, borrowed_options>, std::list<int>>);
    static_assert(not std::constructible_from<any_view<int, copyable_options>, std::list<int>>);

    static_assert(std::constructible_from<any_view<int, borrowed_options>, std::list<int>&>);
    static_assert(std::constructible_from<any_view<int, copyable_options>, std::list<int>&>);
}

TEST(SfinaeTest, deque) {
    static_assert(std::constructible_from<any_view<int, random_access_options>, std::deque<int>>);
    static_assert(not std::constructible_from<any_view<int, contiguous_options>, std::deque<int>>);
    static_assert(std::constructible_from<any_view<int, sized_options>, std::deque<int>>);
    static_assert(not std::constructible_from<any_view<int, borrowed_options>, std::deque<int>>);
    static_assert(not std::constructible_from<any_view<int, copyable_options>, std::deque<int>>);

    static_assert(std::constructible_from<any_view<int, borrowed_options>, std::deque<int>&>);
    static_assert(std::constructible_from<any_view<int, copyable_options>, std::deque<int>&>);
}

TEST(SfinaeTest, vector) {
    static_assert(std::constructible_from<any_view<int, contiguous_options>, std::vector<int>>);
    static_assert(std::constructible_from<any_view<int, sized_options>, std::vector<int>>);
    static_assert(not std::constructible_from<any_view<int, borrowed_options>, std::vector<int>>);
    static_assert(not std::constructible_from<any_view<int, copyable_options>, std::vector<int>>);

    static_assert(std::constructible_from<any_view<int, borrowed_options>, std::vector<int>&>);
    static_assert(std::constructible_from<any_view<int, copyable_options>, std::vector<int>&>);
}

template <class BaseTraitsT>
struct traits : BaseTraitsT {
    using reference_type = bool;
};

TEST(SfinaeTest, vector_of_bool) {
#if BEMAN_ANY_VIEW_USE_FLAGS()
    static_assert(std::constructible_from<any_view<bool, random_access_options, bool>, std::vector<bool>>);
    static_assert(std::constructible_from<any_view<bool, sized_options, bool>, std::vector<bool>>);
    static_assert(not std::constructible_from<any_view<bool, borrowed_options, bool>, std::vector<bool>>);
    static_assert(not std::constructible_from<any_view<bool, copyable_options, bool>, std::vector<bool>>);

    static_assert(std::constructible_from<any_view<bool, borrowed_options, bool>, std::vector<bool>&>);
    static_assert(std::constructible_from<any_view<bool, copyable_options, bool>, std::vector<bool>&>);
#elif BEMAN_ANY_VIEW_USE_TRAITS()
    static_assert(std::constructible_from<any_view<bool, traits<random_access_options>>, std::vector<bool>>);
    static_assert(std::constructible_from<any_view<bool, traits<sized_options>>, std::vector<bool>>);
    static_assert(not std::constructible_from<any_view<bool, traits<borrowed_options>>, std::vector<bool>>);
    static_assert(not std::constructible_from<any_view<bool, traits<copyable_options>>, std::vector<bool>>);

    static_assert(std::constructible_from<any_view<bool, traits<borrowed_options>>, std::vector<bool>&>);
    static_assert(std::constructible_from<any_view<bool, traits<copyable_options>>, std::vector<bool>&>);
#elif BEMAN_ANY_VIEW_USE_NAMED()
    static_assert(
        std::constructible_from<any_view<bool,
                                         {.reference_type          = type<bool>,
                                          .iterator_concept        = type<std::random_access_iterator_tag>,
                                          .BEMAN_ANY_VIEW_OPTION() = move_only_options.BEMAN_ANY_VIEW_OPTION()}>,
                                std::vector<bool>>);
    static_assert(
        std::constructible_from<any_view<bool,
                                         {.reference_type          = type<bool>,
                                          .sized                   = true,
                                          .BEMAN_ANY_VIEW_OPTION() = move_only_options.BEMAN_ANY_VIEW_OPTION()}>,
                                std::vector<bool>>);
    static_assert(
        std::constructible_from<any_view<bool,
                                         {.reference_type          = type<bool>,
                                          .BEMAN_ANY_VIEW_OPTION() = move_only_options.BEMAN_ANY_VIEW_OPTION()}>,
                                std::vector<bool>>);
    static_assert(
        not std::constructible_from<any_view<bool,
                                             {.reference_type          = type<bool>,
                                              .BEMAN_ANY_VIEW_OPTION() = move_only_options.BEMAN_ANY_VIEW_OPTION(),
                                              .borrowed                = true}>,
                                    std::vector<bool>>);
    static_assert(
        not std::constructible_from<any_view<bool,
                                             {.reference_type          = type<bool>,
                                              .BEMAN_ANY_VIEW_OPTION() = copyable_options.BEMAN_ANY_VIEW_OPTION(),
                                              .borrowed                = true}>,
                                    std::vector<bool>>);

    static_assert(
        std::constructible_from<any_view<bool,
                                         {.reference_type          = type<bool>,
                                          .BEMAN_ANY_VIEW_OPTION() = move_only_options.BEMAN_ANY_VIEW_OPTION(),
                                          .borrowed                = true}>,
                                std::vector<bool>&>);
    static_assert(
        std::constructible_from<any_view<bool,
                                         {.reference_type          = type<bool>,
                                          .BEMAN_ANY_VIEW_OPTION() = copyable_options.BEMAN_ANY_VIEW_OPTION(),
                                          .borrowed                = true}>,
                                std::vector<bool>&>);
#endif
}

TEST(SfinaeTest, span) {
    static_assert(std::constructible_from<any_view<int, contiguous_options>, std::span<int>>);
    static_assert(std::constructible_from<any_view<int, sized_options>, std::span<int>>);
    static_assert(std::constructible_from<any_view<int, borrowed_options>, std::span<int>>);
    static_assert(std::constructible_from<any_view<int, copyable_options>, std::span<int>>);
}

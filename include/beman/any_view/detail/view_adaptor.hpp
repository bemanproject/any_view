// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_VIEW_ADAPTOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_VIEW_ADAPTOR_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/detail/adaptor_base.hpp>
#include <beman/any_view/detail/iterator_adaptor.hpp>
#include <beman/any_view/detail/no_unique_address.hpp>

#include <ranges>

namespace beman::any_view::detail {

template <any_view_options OptsV, std::ranges::view ViewT>
struct view_adaptor : adaptor_base {
    static constexpr any_view_options options = OptsV;

    using view_type = ViewT;

    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS ViewT view;
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_VIEW_ADAPTOR_HPP

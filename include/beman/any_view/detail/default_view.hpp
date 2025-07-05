// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_DEFAULT_VIEW_HPP
#define BEMAN_ANY_VIEW_DETAIL_DEFAULT_VIEW_HPP

#include <beman/any_view/detail/unreachable_iterator.hpp>

#include <ranges>

namespace beman::any_view::detail {

template <class ElementT, class RefT, class RValueRefT, class DiffT>
class default_view : public std::ranges::subrange<unreachable_iterator<ElementT, RefT, RValueRefT, DiffT>> {
  public:
    using std::ranges::subrange<unreachable_iterator<ElementT, RefT, RValueRefT, DiffT>>::subrange;
    using std::ranges::subrange<unreachable_iterator<ElementT, RefT, RValueRefT, DiffT>>::operator=;
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_DEFAULT_VIEW_HPP

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

#include <beman/any_view/any_view_options.hpp>
#include <beman/any_view/concepts.hpp>
#include <beman/any_view/detail/default_view.hpp>
#include <beman/any_view/detail/iterator_adaptor.hpp>
#include <beman/any_view/detail/iterator_policies.hpp>
#include <beman/any_view/detail/no_unique_address.hpp>
#include <beman/any_view/detail/type_traits.hpp>
#include <beman/any_view/detail/vtable.hpp>

#include <iterator>

namespace beman::any_view::detail {

// [range.any.iterator]

template <std::derived_from<std::input_iterator_tag> IterTagT, bool IsRefV>
struct iterator_category_type {
    using iterator_category = std::input_iterator_tag;
};

template <bool IsRefV>
struct iterator_category_type<std::input_iterator_tag, IsRefV> {};

template <>
struct iterator_category_type<std::contiguous_iterator_tag, true> {
    using iterator_category = std::random_access_iterator_tag;
};

template <std::derived_from<std::forward_iterator_tag> IterTagT>
struct iterator_category_type<IterTagT, true> {
    using iterator_category = IterTagT;
};

template <class ElementT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
class iterator : public iterator_category_type<iterator_concept_t<OptsV>, std::is_reference_v<RefT>> {
    using cache_type = std::conditional_t<flag_is_set<OptsV, any_view_options::forward>, iter_cache_t<RefT>, no_cache>;
    using polymorphic_type = polymorphic_iterator<RefT, RValueRefT, DiffT, OptsV>;

    polymorphic_type                            poly;
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS cache_type cache;

    constexpr cache_type make_cache() const {
        if constexpr (std::same_as<cache_type, no_cache>) {
            return cache_type{};
        } else {
            return detail::fn<cache_policy<RefT>, polymorphic_type>(poly);
        }
    }

  public:
    using iterator_concept = iterator_concept_t<OptsV>;
    using value_type       = std::remove_cv_t<ElementT>;
    using difference_type  = DiffT;

    constexpr explicit iterator(polymorphic_type&& poly) : poly(std::move(poly)), cache(make_cache()) {}

    constexpr iterator() noexcept
        requires flag_is_set<OptsV, any_view_options::forward>
        : poly(iterator_adaptor_for<default_view<ElementT, RefT, RValueRefT, DiffT>>{}), cache(make_cache()) {}

    constexpr iterator(const iterator&)
        requires flag_is_set<OptsV, any_view_options::forward>
    = default;

    constexpr iterator(iterator&&) noexcept = default;

    constexpr iterator& operator=(const iterator&)
        requires flag_is_set<OptsV, any_view_options::forward>
    = default;

    constexpr iterator& operator=(iterator&&) noexcept = default;

    [[nodiscard]] constexpr RefT operator*() const {
        if constexpr (std::same_as<cache_type, no_cache>) {
            return detail::fn<dereference_policy<RefT>, polymorphic_type>(poly);
        } else {
            return *cache;
        }
    }

    [[nodiscard]] friend constexpr RValueRefT iter_move(const iterator& other) {
        return detail::fn<iter_move_policy<RValueRefT>, polymorphic_type>(other.poly);
    }

    [[nodiscard]] constexpr std::add_pointer_t<RefT> operator->() const
        requires flag_is_set<OptsV, any_view_options::contiguous>
    {
        if constexpr (std::same_as<cache_type, no_cache>) {
            return detail::fn<to_address_policy<RefT>, polymorphic_type>(poly);
        } else {
            return cache;
        }
    }

    constexpr iterator& operator++() {
        if constexpr (std::same_as<cache_type, no_cache>) {
            detail::fn<increment_policy, polymorphic_type>(poly);
        } else {
            cache = detail::fn<next_policy<RefT>, polymorphic_type>(poly);
        }
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    [[nodiscard]] constexpr iterator operator++(int)
        requires flag_is_set<OptsV, any_view_options::forward>
    {
        const auto other = *this;
        ++*this;
        return other;
    }

    [[nodiscard]] constexpr bool operator==(const iterator& other) const
        requires flag_is_set<OptsV, any_view_options::forward>
    {
        return detail::fn<equality_compare_policy, polymorphic_type>(poly, other.poly);
    }

    constexpr iterator& operator--()
        requires flag_is_set<OptsV, any_view_options::bidirectional>
    {
        if constexpr (std::same_as<cache_type, no_cache>) {
            detail::fn<decrement_policy, polymorphic_type>(poly);
        } else {
            cache = detail::fn<prev_policy<RefT>, polymorphic_type>(poly);
        }
        return *this;
    }

    [[nodiscard]] constexpr iterator operator--(int)
        requires flag_is_set<OptsV, any_view_options::bidirectional>
    {
        const auto other = *this;
        --*this;
        return other;
    }

    [[nodiscard]] constexpr std::partial_ordering operator<=>(const iterator& other) const
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        return detail::fn<three_way_compare_policy, polymorphic_type>(poly, other.poly);
    }

    [[nodiscard]] constexpr difference_type operator-(const iterator& other) const
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        return detail::fn<subtract_policy<DiffT>, polymorphic_type>(poly, other.poly);
    }

    constexpr iterator& operator+=(difference_type offset)
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        if constexpr (std::same_as<cache_type, no_cache>) {
            detail::fn<compound_add_policy<DiffT>, polymorphic_type>(poly, offset);
        } else {
            cache = detail::fn<advance_policy<RefT, DiffT>, polymorphic_type>(poly, offset);
        }
        return *this;
    }

    [[nodiscard]] constexpr iterator operator+(difference_type offset) const
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        auto other = *this;
        other += offset;
        return other;
    }

    [[nodiscard]] friend constexpr iterator operator+(difference_type offset, const iterator& other)
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        return other + offset;
    }

    constexpr iterator& operator-=(difference_type offset)
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        // TODO: should virtual function for operator-= be provided to avoid signed overflow for max difference type?
        *this += -offset;
        return *this;
    }

    [[nodiscard]] constexpr iterator operator-(difference_type offset) const
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        auto other = *this;
        other -= offset;
        return other;
    }

    [[nodiscard]] constexpr RefT operator[](difference_type offset) const
        requires flag_is_set<OptsV, any_view_options::random_access>
    {
        return *(*this + offset);
    }

    [[nodiscard]] constexpr bool operator==(std::default_sentinel_t sentinel) const {
        if constexpr (std::same_as<cache_type, no_cache>) {
            return detail::fn<sentinel_compare_policy, polymorphic_type>(poly, sentinel);
        } else {
            return cache == cache_type{};
        }
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

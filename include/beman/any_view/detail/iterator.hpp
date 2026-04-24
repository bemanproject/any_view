// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

#include <beman/any_view/detail/default_view.hpp>
#include <beman/any_view/detail/polymorphic_iterator.hpp>

namespace beman::any_view::detail {

template <any_view_options OptsV>
[[nodiscard]] consteval auto get_iterator_concept() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, contiguous>) {
        return std::contiguous_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, random_access>) {
        return std::random_access_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, bidirectional>) {
        return std::bidirectional_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, forward>) {
        return std::forward_iterator_tag{};
    } else if constexpr (flag_is_set<OptsV, input>) {
        return std::input_iterator_tag{};
    }
}

template <any_view_options OptsV>
using iterator_concept_t = decltype(get_iterator_concept<OptsV>());

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
    static constexpr bool forward       = flag_is_set<OptsV, any_view_options::forward>;
    static constexpr bool bidirectional = flag_is_set<OptsV, any_view_options::bidirectional>;
    static constexpr bool random_access = flag_is_set<OptsV, any_view_options::random_access>;
    static constexpr bool contiguous    = flag_is_set<OptsV, any_view_options::contiguous>;

    // do not cache an iterator reference without multi-pass guarantee
    using cache_type       = std::conditional_t<forward, iter_cache_t<RefT>, no_cache>;
    using polymorphic_type = polymorphic_iterator<RefT, RValueRefT, DiffT, OptsV>;

    static constexpr bool has_cache = not std::is_same_v<cache_type, no_cache>;

    // store an index to avoid runtime dispatch for advancing a random access iterator with no cache
    using cache_or_index_type = std::conditional_t<random_access and not has_cache, DiffT, cache_type>;

    static constexpr bool has_index = std::is_same_v<cache_or_index_type, DiffT>;

    template <protocol ProtocolT>
    static constexpr auto dispatch = detail::dispatch<ProtocolT, polymorphic_type>;

    polymorphic_type poly{std::in_place_type<iterator_adaptor_for<default_view<ElementT, RefT, RValueRefT, DiffT>>>};
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS cache_or_index_type cache_or_index{make_cache_or_index()};

    constexpr cache_or_index_type make_cache_or_index() const {
        if constexpr (has_cache) {
            return dispatch<cache_t<RefT>>(poly);
        } else {
            return {};
        }
    }

  public:
    using iterator_concept = iterator_concept_t<OptsV>;
    using value_type       = std::remove_cv_t<ElementT>;
    using difference_type  = DiffT;

    template <class GetPolyT>
        requires std::is_invocable_r_v<polymorphic_type, GetPolyT>
    constexpr explicit iterator(GetPolyT get_poly) : poly(get_poly()) {}

    constexpr iterator() noexcept
        requires forward
    = default;

    constexpr iterator(const iterator&)
        requires forward
    = default;

    constexpr iterator(iterator&&) noexcept = default;

    constexpr iterator& operator=(const iterator&)
        requires forward
    = default;

    constexpr iterator& operator=(iterator&&) noexcept = default;

    [[nodiscard]] constexpr RefT operator*() const {
        if constexpr (has_cache) {
            return *cache_or_index;
        } else if constexpr (has_index) {
            return dispatch<dereference_at_t<RefT, DiffT>>(poly, cache_or_index);
        } else {
            return dispatch<dereference_t<RefT>>(poly);
        }
    }

    [[nodiscard]] constexpr friend RValueRefT iter_move(const iterator& self) {
        if constexpr (has_cache) {
            return std::ranges::iter_move(self.cache_or_index);
        } else if constexpr (has_index) {
            return dispatch<iter_move_at_t<RValueRefT, DiffT>>(self.poly, self.cache_or_index);
        } else {
            return dispatch<iter_move_t<RValueRefT>>(self.poly);
        }
    }

    [[nodiscard]] constexpr std::add_pointer_t<RefT> operator->() const
        requires contiguous
    {
        static_assert(has_cache, "contiguous iterator requires lvalue reference");
        return cache_or_index;
    }

    constexpr iterator& operator++() {
        if constexpr (contiguous or has_index) {
            ++cache_or_index;
            if constexpr (contiguous) {
                dispatch<sync_t<RefT>>(poly, cache_or_index);
            }
        } else if constexpr (has_cache) {
            cache_or_index = dispatch<next_t<RefT>>(poly);
        } else {
            dispatch<increment_t>(poly);
        }
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    [[nodiscard]] constexpr iterator operator++(int)
        requires forward
    {
        const auto other = *this;
        ++*this;
        return other;
    }

    [[nodiscard]] constexpr bool operator==(const iterator& other) const
        requires forward
    {
        if constexpr (contiguous or has_index) {
            return cache_or_index == other.cache_or_index;
        } else {
            return dispatch<equality_compare_t>(poly, other.poly);
        }
    }

    constexpr iterator& operator--()
        requires bidirectional
    {
        if constexpr (contiguous or has_index) {
            --cache_or_index;
            if constexpr (contiguous) {
                dispatch<sync_t<RefT>>(poly, cache_or_index);
            }
        } else if constexpr (has_cache) {
            cache_or_index = dispatch<prev_t<RefT>>(poly);
        } else {
            dispatch<decrement_t>(poly);
        }
        return *this;
    }

    [[nodiscard]] constexpr iterator operator--(int)
        requires bidirectional
    {
        const auto other = *this;
        --*this;
        return other;
    }

    [[nodiscard]] constexpr std::partial_ordering operator<=>(const iterator& other) const
        requires random_access
    {
        if constexpr (contiguous or has_index) {
            return cache_or_index <=> other.cache_or_index;
        } else {
            return dispatch<three_way_compare_t>(poly, other.poly);
        }
    }

    [[nodiscard]] constexpr difference_type operator-(const iterator& other) const
        requires random_access
    {
        if constexpr (contiguous or has_index) {
            return cache_or_index - other.cache_or_index;
        } else {
            return dispatch<subtract_t<DiffT>>(poly, other.poly);
        }
    }

    constexpr iterator& operator+=(difference_type offset)
        requires random_access
    {
        if constexpr (contiguous or has_index) {
            cache_or_index += offset;
            if constexpr (contiguous) {
                dispatch<sync_t<RefT>>(poly, cache_or_index);
            }
        } else {
            static_assert(has_cache, "random access iterator with no index has cache");
            cache_or_index = dispatch<advance_t<RefT, DiffT>>(poly, offset);
        }
        return *this;
    }

    [[nodiscard]] constexpr iterator operator+(difference_type offset) const
        requires random_access
    {
        auto other = *this;
        other += offset;
        return other;
    }

    [[nodiscard]] constexpr friend iterator operator+(difference_type offset, const iterator& other)
        requires random_access
    {
        return other + offset;
    }

    constexpr iterator& operator-=(difference_type offset)
        requires random_access
    {
        // TODO: should virtual function for operator-= be provided to avoid signed overflow for max difference type?
        *this += -offset;
        return *this;
    }

    [[nodiscard]] constexpr iterator operator-(difference_type offset) const
        requires random_access
    {
        auto other = *this;
        other -= offset;
        return other;
    }

    [[nodiscard]] constexpr RefT operator[](difference_type offset) const
        requires random_access
    {
        return *(*this + offset);
    }

    [[nodiscard]] constexpr bool operator==(std::default_sentinel_t) const {
        // sentinel comparison must dispatch for a contiguous iterator
        if constexpr (has_cache and not contiguous) {
            return cache_or_index == cache_type{};
        } else {
            return dispatch<sentinel_compare_t>(poly);
        }
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

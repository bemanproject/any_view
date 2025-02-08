// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

#include <beman/any_view/concepts.hpp>
#include <beman/any_view/detail/intrusive_small_ptr.hpp>
#include <beman/any_view/detail/iterator_adaptor.hpp>
#include <beman/any_view/detail/iterator_interface.hpp>
#include <beman/any_view/detail/type_traits.hpp>

#include <compare>
#include <type_traits>
#include <iterator>

namespace beman::any_view::detail {

template <class IterConceptT, class ElementT, class RefT, class RValueRefT, class DiffT>
class iterator {
    using reference        = RefT;
    using rvalue_reference = RValueRefT;
    using pointer          = std::add_pointer_t<RefT>;

    struct range_traits {
        using iterator_concept      = IterConceptT;
        using reference_type        = RefT;
        using rvalue_reference_type = RValueRefT;
        using difference_type       = DiffT;
    };

    static constexpr bool forward       = std::derived_from<IterConceptT, std::forward_iterator_tag>;
    static constexpr bool bidirectional = std::derived_from<IterConceptT, std::bidirectional_iterator_tag>;
    static constexpr bool random_access = std::derived_from<IterConceptT, std::random_access_iterator_tag>;
    static constexpr bool contiguous    = std::derived_from<IterConceptT, std::contiguous_iterator_tag>;

    using interface_type = iterator_interface<ElementT, RefT, RValueRefT, DiffT>;
    // inplace storage sufficient for a vtable pointer and two pointers
    intrusive_small_ptr<interface_type, 3 * sizeof(void*)> iterator_ptr;

    template <class IteratorT, class SentinelT>
    using adaptor_type = detail::iterator_adaptor<ElementT, reference, rvalue_reference, DiffT, IteratorT, SentinelT>;

    template <class IteratorT, class SentinelT>
    static constexpr auto get_noexcept() {
        return std::is_nothrow_constructible_v<adaptor_type<IteratorT, SentinelT>, IteratorT, SentinelT>;
    }

    template <class IteratorT, class SentinelT>
    static constexpr auto get_in_place_adaptor_type() {
        return std::in_place_type<adaptor_type<IteratorT, SentinelT>>;
    }

  public:
    using iterator_concept = IterConceptT;
    using element_type     = ElementT;
    using difference_type  = DiffT;

    template <detail::iterator_compatible_with<range_traits> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    constexpr iterator(IteratorT iterator, SentinelT sentinel) noexcept(get_noexcept<IteratorT, SentinelT>())
        : iterator_ptr(get_in_place_adaptor_type<IteratorT, SentinelT>(), std::move(iterator), std::move(sentinel)) {}

    constexpr iterator() noexcept
        requires forward
        : iterator(pointer(nullptr), pointer(nullptr)) {}

    constexpr iterator(const iterator&)
        requires forward
    = default;

    constexpr iterator(iterator&&) noexcept = default;

    constexpr auto operator=(const iterator&) -> iterator&
        requires forward
    = default;

    constexpr auto operator=(iterator&&) noexcept -> iterator& = default;

    [[nodiscard]] constexpr auto operator*() const -> reference { return **iterator_ptr; }

    friend constexpr auto iter_move(const iterator& other) -> rvalue_reference {
        return other.iterator_ptr->iter_move();
    }

    [[nodiscard]] constexpr auto operator->() const -> pointer
        requires contiguous
    {
        return std::to_address(*iterator_ptr);
    }

    constexpr auto operator++() -> iterator& {
        ++*iterator_ptr;
        return *this;
    }

    constexpr auto operator++(int) -> void { ++*this; }

    [[nodiscard]] constexpr auto operator++(int) -> iterator
        requires forward
    {
        const auto other = *this;
        ++*this;
        return other;
    }

    [[nodiscard]] constexpr auto operator==(const iterator& other) const -> bool
        requires forward
    {
        return *iterator_ptr == *other.iterator_ptr;
    }

    constexpr auto operator--() -> iterator&
        requires bidirectional
    {
        --*iterator_ptr;
        return *this;
    }

    [[nodiscard]] constexpr auto operator--(int) -> iterator
        requires bidirectional
    {
        const auto other = *this;
        --*this;
        return other;
    }

    [[nodiscard]] constexpr auto operator<=>(const iterator& other) const -> std::partial_ordering
        requires random_access
    {
        return *iterator_ptr <=> *other.iterator_ptr;
    }

    [[nodiscard]] constexpr auto operator-(const iterator& other) const -> difference_type
        requires random_access
    {
        return *iterator_ptr - *other.iterator_ptr;
    }

    constexpr auto operator+=(difference_type offset) -> iterator&
        requires random_access
    {
        *iterator_ptr += offset;
        return *this;
    }

    [[nodiscard]] constexpr auto operator+(difference_type offset) const -> iterator
        requires random_access
    {
        auto other = *this;
        other += offset;
        return other;
    }

    [[nodiscard]] friend constexpr auto operator+(difference_type offset, const iterator& other) -> iterator
        requires random_access
    {
        return other + offset;
    }

    constexpr auto operator-=(difference_type offset) -> iterator&
        requires random_access
    {
        *iterator_ptr -= offset;
        return *this;
    }

    [[nodiscard]] constexpr auto operator-(difference_type offset) const -> iterator
        requires random_access
    {
        auto other = *this;
        other -= offset;
        return other;
    }

    [[nodiscard]] constexpr auto operator[](difference_type offset) const -> reference
        requires random_access
    {
        return *(*this + offset);
    }

    [[nodiscard]] constexpr auto operator==(std::default_sentinel_t sentinel) const -> bool {
        return *iterator_ptr == sentinel;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_HPP

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ANY_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ANY_ITERATOR_HPP

#include <beman/any_view/concepts.hpp>
#include <beman/any_view/detail/intrusive_small_ptr.hpp>
#include <beman/any_view/detail/iterator_adaptor.hpp>

namespace beman::any_view::detail {

template <class IterConceptT, class ElementT, class RefT, class RValueRefT, class DiffT>
class any_iterator {
    using reference        = RefT;
    using rvalue_reference = RValueRefT;
    using pointer          = std::add_pointer_t<RefT>;

    using interface_type = iterator_interface<ElementT, RefT, RValueRefT, DiffT>;
    // inplace storage sufficient for a vtable pointer and two pointers
    intrusive_small_ptr<interface_type, 3 * sizeof(void*)> iterator_ptr;

    template <class IteratorT, class SentinelT>
    static consteval auto get_in_place_adaptor_type() {
        return std::in_place_type<
            detail::iterator_adaptor<ElementT, reference, rvalue_reference, DiffT, IteratorT, SentinelT>>;
    }

  public:
    using iterator_concept = IterConceptT;
    using element_type     = ElementT;
    using difference_type  = DiffT;

    template <detail::any_compatible_iterator<any_iterator> IteratorT, std::sentinel_for<IteratorT> SentinelT>
    constexpr any_iterator(IteratorT iterator, SentinelT sentinel)
        : iterator_ptr(get_in_place_adaptor_type<IteratorT, SentinelT>(), std::move(iterator), std::move(sentinel)) {}

    constexpr any_iterator() noexcept
        requires std::derived_from<IterConceptT, std::forward_iterator_tag>
        : any_iterator(pointer(nullptr), pointer(nullptr)) {}

    constexpr any_iterator(const any_iterator&)
        requires std::derived_from<IterConceptT, std::forward_iterator_tag>
    = default;

    constexpr any_iterator(any_iterator&&) noexcept = default;

    constexpr any_iterator& operator=(const any_iterator&)
        requires std::derived_from<IterConceptT, std::forward_iterator_tag>
    = default;

    constexpr any_iterator& operator=(any_iterator&&) noexcept = default;

    [[nodiscard]] constexpr reference operator*() const { return **iterator_ptr; }

    [[nodiscard]] friend constexpr rvalue_reference iter_move(const any_iterator& other) {
        return other.iterator_ptr->iter_move();
    }

    [[nodiscard]] constexpr pointer operator->() const
        requires std::derived_from<IterConceptT, std::contiguous_iterator_tag>
    {
        return std::to_address(*iterator_ptr);
    }

    constexpr any_iterator& operator++() {
        ++*iterator_ptr;
        return *this;
    }

    constexpr void operator++(int) { ++*this; }

    [[nodiscard]] constexpr any_iterator operator++(int)
        requires std::derived_from<IterConceptT, std::forward_iterator_tag>
    {
        const auto other = *this;
        ++*this;
        return other;
    }

    [[nodiscard]] constexpr bool operator==(const any_iterator& other) const
        requires std::derived_from<IterConceptT, std::forward_iterator_tag>
    {
        return *iterator_ptr == *other.iterator_ptr;
    }

    constexpr any_iterator& operator--()
        requires std::derived_from<IterConceptT, std::bidirectional_iterator_tag>
    {
        --*iterator_ptr;
        return *this;
    }

    [[nodiscard]] constexpr any_iterator operator--(int)
        requires std::derived_from<IterConceptT, std::bidirectional_iterator_tag>
    {
        const auto other = *this;
        --*this;
        return other;
    }

    [[nodiscard]] constexpr std::partial_ordering operator<=>(const any_iterator& other) const
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        return *iterator_ptr <=> *other.iterator_ptr;
    }

    [[nodiscard]] constexpr difference_type operator-(const any_iterator& other) const
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        return *iterator_ptr - *other.iterator_ptr;
    }

    constexpr any_iterator& operator+=(difference_type offset)
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        *iterator_ptr += offset;
        return *this;
    }

    [[nodiscard]] constexpr any_iterator operator+(difference_type offset) const
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        auto other = *this;
        other += offset;
        return other;
    }

    [[nodiscard]] friend constexpr any_iterator operator+(difference_type offset, const any_iterator& other)
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        return other + offset;
    }

    constexpr any_iterator& operator-=(difference_type offset)
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        // TODO: should virtual function for operator-= be provided to avoid signed overflow for max difference type?
        *this += -offset;
        return *this;
    }

    [[nodiscard]] constexpr any_iterator operator-(difference_type offset) const
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        auto other = *this;
        other -= offset;
        return other;
    }

    [[nodiscard]] constexpr reference operator[](difference_type offset) const
        requires std::derived_from<IterConceptT, std::random_access_iterator_tag>
    {
        return *(*this + offset);
    }

    [[nodiscard]] constexpr bool operator==(std::default_sentinel_t sentinel) const {
        return *iterator_ptr == sentinel;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ANY_ITERATOR_HPP

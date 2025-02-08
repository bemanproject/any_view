// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

#include <beman/any_view/detail/iterator_interface.hpp>
#include <beman/any_view/detail/utility.hpp>

#include <compare>
#include <iterator>

namespace beman::any_view::detail {

template <class ElementT,
          class RefT,
          class RValueRefT,
          class DiffT,
          std::input_or_output_iterator IteratorT,
          std::sentinel_for<IteratorT>  SentinelT>
class iterator_adaptor : public iterator_interface<ElementT, RefT, RValueRefT, DiffT> {
    [[no_unique_address]] IteratorT iterator;
    [[no_unique_address]] SentinelT sentinel;

    using iterator_interface = detail::iterator_interface<ElementT, RefT, RValueRefT, DiffT>;
    using pointer            = std::add_pointer_t<RefT>;

    static constexpr auto get_noexcept() {
        return std::is_nothrow_move_constructible_v<IteratorT> and std::is_nothrow_move_constructible_v<SentinelT>;
    }

    static constexpr bool forward       = std::forward_iterator<IteratorT>;
    static constexpr bool bidirectional = std::bidirectional_iterator<IteratorT>;
    static constexpr bool random_access = std::random_access_iterator<IteratorT>;
    static constexpr bool contiguous    = std::contiguous_iterator<IteratorT>;

    static constexpr auto down_cast(const iterator_interface& other) {
        return dynamic_cast<const iterator_adaptor*>(std::addressof(other));
    }

  public:
    constexpr iterator_adaptor(IteratorT&& iterator, SentinelT&& sentinel) noexcept(get_noexcept())
        : iterator(std::move(iterator)), sentinel(std::move(sentinel)) {}

    auto copy_to(void* destination) const -> void override {
        if constexpr (std::copy_constructible<IteratorT> and std::copy_constructible<SentinelT>) {
            ::new (destination) iterator_adaptor(*this);
        } else {
            unreachable();
        }
    }

    auto move_to(void* destination) noexcept -> void override {
        if constexpr (std::is_nothrow_move_constructible_v<IteratorT> and
                      std::is_nothrow_move_constructible_v<SentinelT>) {
            ::new (destination) iterator_adaptor(std::move(*this));
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto copy() const -> iterator_adaptor* override {
        if constexpr (std::copy_constructible<IteratorT> and std::copy_constructible<SentinelT>) {
            return new iterator_adaptor(*this);
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator*() const -> RefT override { return *iterator; }

    [[nodiscard]] constexpr auto iter_move() const -> RValueRefT override { return std::ranges::iter_move(iterator); }

    [[nodiscard]] constexpr auto operator->() const -> pointer override {
        if constexpr (contiguous) {
            return std::to_address(iterator);
        } else {
            unreachable();
        }
    }

    constexpr auto operator++() -> void override { ++iterator; }

    [[nodiscard]] constexpr auto operator==(const iterator_interface& other) const -> bool override {
        if constexpr (forward) {
            if (const auto adaptor = down_cast(other)) {
                return iterator == adaptor->iterator;
            }
        }

        return false;
    }

    constexpr auto operator--() -> void override {
        if constexpr (bidirectional) {
            --iterator;
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator<=>(const iterator_interface& other) const -> std::partial_ordering override {
        if constexpr (random_access) {
            if (const auto adaptor = down_cast(other)) {
                return iterator <=> adaptor->iterator;
            }
        }

        return std::partial_ordering::unordered;
    }

    [[nodiscard]] constexpr auto operator-(const iterator_interface& other) const -> DiffT override {
        if constexpr (random_access) {
            if (const auto adaptor = down_cast(other)) {
                return iterator - adaptor->iterator;
            }
        }

        unreachable();
    }

    constexpr auto operator+=(DiffT offset) -> void override {
        if constexpr (random_access) {
            iterator += offset;
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator==(std::default_sentinel_t) const -> bool override {
        return iterator == sentinel;
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

#include <beman/any_view/detail/iterator_interface.hpp>

#include <compare>
#include <iterator>
#include <utility>

namespace beman::any_view::detail {

template <class IterConceptT,
          class ElementT,
          class RefT,
          class RValueRefT,
          class DiffT,
          std::input_or_output_iterator IteratorT,
          std::sentinel_for<IteratorT>  SentinelT>
class iterator_adaptor : public iterator_interface<IterConceptT, ElementT, RefT, RValueRefT, DiffT> {
    [[no_unique_address]] IteratorT iterator;
    [[no_unique_address]] SentinelT sentinel;

    using iterator_interface = detail::iterator_interface<IterConceptT, ElementT, RefT, RValueRefT, DiffT>;
    using pointer            = std::add_pointer_t<RefT>;

    static constexpr auto get_noexcept() {
        return std::is_nothrow_move_constructible_v<IteratorT> and std::is_nothrow_move_constructible_v<SentinelT>;
    }

    static constexpr bool forward       = std::derived_from<IterConceptT, std::forward_iterator_tag>;
    static constexpr bool bidirectional = std::derived_from<IterConceptT, std::bidirectional_iterator_tag>;
    static constexpr bool random_access = std::derived_from<IterConceptT, std::random_access_iterator_tag>;
    static constexpr bool contiguous    = std::derived_from<IterConceptT, std::contiguous_iterator_tag>;

  public:
    constexpr iterator_adaptor(IteratorT&& iterator, SentinelT&& sentinel) noexcept(get_noexcept())
        : iterator(std::move(iterator)), sentinel(std::move(sentinel)) {}

    auto copy_to(void* destination) const -> void override {
        if constexpr (std::copy_constructible<IteratorT> and std::copy_constructible<SentinelT>) {
            ::new (destination) iterator_adaptor(*this);
        } else {
            std::unreachable();
        }
    }

    auto move_to(void* destination) noexcept -> void override {
        if constexpr (std::is_nothrow_move_constructible_v<IteratorT> and
                      std::is_nothrow_move_constructible_v<SentinelT>) {
            ::new (destination) iterator_adaptor(std::move(*this));
        } else {
            std::unreachable();
        }
    }

    [[nodiscard]] constexpr auto copy() const -> iterator_adaptor* override {
        if constexpr (std::copy_constructible<IteratorT> and std::copy_constructible<SentinelT>) {
            return new iterator_adaptor(*this);
        } else {
            std::unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator*() const -> RefT override { return *iterator; }

    [[nodiscard]] constexpr auto iter_move() const -> RValueRefT override { return std::ranges::iter_move(iterator); }

    [[nodiscard]] constexpr auto operator->() const -> pointer override {
        if constexpr (contiguous) {
            return std::to_address(iterator);
        } else {
            std::unreachable();
        }
    }

    constexpr auto operator++() -> void override { ++iterator; }

    [[nodiscard]] constexpr auto operator==(const iterator_interface& other) const -> bool override {
        if constexpr (forward) {
            if (type() == other.type()) {
                return iterator == static_cast<const iterator_adaptor&>(other).iterator;
            }
        }

        return false;
    }

    constexpr auto operator--() -> void override {
        if constexpr (bidirectional) {
            --iterator;
        } else {
            std::unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator<=>(const iterator_interface& other) const -> std::partial_ordering override {
        if constexpr (random_access) {
            if (type() == other.type()) {
                return iterator <=> static_cast<const iterator_adaptor&>(other).iterator;
            }
        }

        return std::partial_ordering::unordered;
    }

    [[nodiscard]] constexpr auto operator-(const iterator_interface& other) const -> DiffT override {
        if constexpr (random_access) {
            if (type() == other.type()) {
                return iterator - static_cast<const iterator_adaptor&>(other).iterator;
            }
        }

        std::unreachable();
    }

    constexpr auto operator+=(DiffT offset) -> void override {
        if constexpr (random_access) {
            iterator += offset;
        } else {
            std::unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator==(std::default_sentinel_t) const -> bool override {
        return iterator == sentinel;
    }

    [[nodiscard]] constexpr auto type() const noexcept -> const std::type_info& override {
        return typeid(iterator_adaptor);
    }
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

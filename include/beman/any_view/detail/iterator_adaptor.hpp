// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

#include <beman/any_view/detail/concepts.hpp>
#include <beman/any_view/detail/iterator_interface.hpp>
#include <beman/any_view/detail/unique_address.hpp>
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
class iterator_adaptor final : public iterator_interface<ElementT, RefT, RValueRefT, DiffT> {
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS() IteratorT iterator;
    BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS() SentinelT sentinel;

    using iterator_interface = detail::iterator_interface<ElementT, RefT, RValueRefT, DiffT>;
    using pointer            = std::add_pointer_t<RefT>;

    static constexpr bool cached = not std::same_as<iter_cache_t<RefT>, no_cache>;

    static constexpr auto get_noexcept() {
        return std::is_nothrow_move_constructible_v<IteratorT> and std::is_nothrow_move_constructible_v<SentinelT>;
    }

    static constexpr auto down_cast(const iterator_interface& other) {
        return dynamic_cast<const iterator_adaptor*>(std::addressof(other));
    }

  public:
    constexpr iterator_adaptor() noexcept(std::is_nothrow_default_constructible_v<IteratorT> and
                                          std::is_nothrow_default_constructible_v<SentinelT>) = default;

    constexpr iterator_adaptor(const iterator_adaptor&) = default;

    constexpr iterator_adaptor(iterator_adaptor&&) noexcept(std::is_nothrow_move_constructible_v<IteratorT> and
                                                            std::is_nothrow_move_constructible_v<SentinelT>) = default;

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

    [[nodiscard]] constexpr auto next() -> iter_cache_t<RefT> override {
        if constexpr (std::forward_iterator<IteratorT> and cached) {
            return ++iterator != sentinel ? iter_cache<RefT>::make(*iterator) : iter_cache_t<RefT>{};
        }

        unreachable();
    }

    [[nodiscard]] constexpr auto prev() -> iter_cache_t<RefT> override {
        if constexpr (std::bidirectional_iterator<IteratorT> and cached) {
            return --iterator != sentinel ? iter_cache<RefT>::make(*iterator) : iter_cache_t<RefT>{};
        }

        unreachable();
    }

    [[nodiscard]] constexpr auto next(DiffT n) -> iter_cache_t<RefT> override {
        if constexpr (std::random_access_iterator<IteratorT> and cached) {
            return (iterator += n) != sentinel ? iter_cache<RefT>::make(*iterator) : iter_cache_t<RefT>{};
        }

        unreachable();
    }

    [[nodiscard]] constexpr auto operator*() const -> RefT override { return *iterator; }

    [[nodiscard]] constexpr auto iter_move() const -> RValueRefT override { return std::ranges::iter_move(iterator); }

    [[nodiscard]] constexpr auto operator->() const -> pointer override {
        if constexpr (std::contiguous_iterator<IteratorT> and
                      any_compatible_contiguous_reference<std::iter_reference_t<IteratorT>, RefT>) {
            return std::to_address(iterator);
        } else {
            unreachable();
        }
    }

    constexpr auto operator++() -> void override { ++iterator; }

    [[nodiscard]] constexpr auto operator==(const iterator_interface& other) const -> bool override {
        if constexpr (std::forward_iterator<IteratorT>) {
            if (const auto adaptor = down_cast(other)) {
                return iterator == adaptor->iterator;
            }
        }

        return false;
    }

    constexpr auto operator--() -> void override {
        if constexpr (std::bidirectional_iterator<IteratorT>) {
            --iterator;
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator<=>(const iterator_interface& other) const -> std::partial_ordering override {
        if constexpr (std::random_access_iterator<IteratorT>) {
            if (const auto adaptor = down_cast(other)) {
                // std::__wrap_iter is not three-way comparable in Apple Clang
                if constexpr (std::three_way_comparable<IteratorT>) {
                    return iterator <=> adaptor->iterator;
                } else {
                    return iterator < adaptor->iterator    ? std::partial_ordering::less
                           : iterator > adaptor->iterator  ? std::partial_ordering::greater
                           : iterator == adaptor->iterator ? std::partial_ordering::equivalent
                                                           : std::partial_ordering::unordered;
                }
            }
        }

        return std::partial_ordering::unordered;
    }

    [[nodiscard]] constexpr auto operator-(const iterator_interface& other) const -> DiffT override {
        if constexpr (std::random_access_iterator<IteratorT>) {
            if (const auto adaptor = down_cast(other)) {
                return iterator - adaptor->iterator;
            }
        }

        unreachable();
    }

    constexpr auto operator+=(DiffT offset) -> void override {
        if constexpr (std::random_access_iterator<IteratorT>) {
            iterator += offset;
        } else {
            unreachable();
        }
    }

    [[nodiscard]] constexpr auto operator==(std::default_sentinel_t) const -> bool override {
        return iterator == sentinel;
    }

    // workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=104653
    constexpr ~iterator_adaptor() noexcept override {}
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_ADAPTOR_HPP

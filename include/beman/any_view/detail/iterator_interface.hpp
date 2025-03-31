// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_ITERATOR_INTERFACE_HPP
#define BEMAN_ANY_VIEW_DETAIL_ITERATOR_INTERFACE_HPP

#include <compare>
#include <iterator>
#include <optional>
#include <type_traits>

namespace beman::any_view::detail {

struct no_cache {};

template <class RefT>
struct iter_cache {
    using type = no_cache;
};

template <class RefT>
    requires std::is_trivially_copyable_v<RefT>
struct iter_cache<RefT> {
    using type = std::optional<RefT>;

    static constexpr auto make(RefT ref) noexcept -> type { return {ref}; }
};

template <class RefT>
struct iter_cache<RefT&> {
    using type = RefT*;

    static constexpr auto make(RefT& ref) noexcept -> type { return std::addressof(ref); }
};

template <class RefT>
using iter_cache_t = iter_cache<RefT>::type;

template <class ElementT, class RefT, class RValueRefT, class DiffT>
class iterator_interface {
    using pointer = std::add_pointer_t<RefT>;

  public:
    // in-place construction
    virtual auto copy_to(void* destination) const -> void    = 0;
    virtual auto move_to(void* destination) noexcept -> void = 0;

    // out-of-place construction
    [[nodiscard]] virtual constexpr auto copy() const -> iterator_interface* = 0;

    // fused increment-compare-dereference
    [[nodiscard]] virtual constexpr auto next() -> iter_cache_t<RefT> = 0;

    [[nodiscard]] virtual constexpr auto prev() -> iter_cache_t<RefT> = 0;

    [[nodiscard]] virtual constexpr auto next(DiffT n) -> iter_cache_t<RefT> = 0;

    // iterator methods
    [[nodiscard]] virtual constexpr auto operator*() const -> RefT = 0;

    [[nodiscard]] virtual constexpr auto iter_move() const -> RValueRefT = 0;

    [[nodiscard]] virtual constexpr auto operator->() const -> pointer = 0;

    virtual constexpr auto operator++() -> void = 0;

    [[nodiscard]] virtual constexpr auto operator==(const iterator_interface& other) const -> bool = 0;

    virtual constexpr auto operator--() -> void = 0;

    [[nodiscard]] virtual constexpr auto
    operator<=>(const iterator_interface& other) const -> std::partial_ordering = 0;

    [[nodiscard]] virtual constexpr auto operator-(const iterator_interface& other) const -> DiffT = 0;

    virtual constexpr auto operator+=(DiffT offset) -> void = 0;

    [[nodiscard]] virtual constexpr auto operator==(std::default_sentinel_t sentinel) const -> bool = 0;

    virtual constexpr ~iterator_interface() noexcept = default;
};

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_ITERATOR_INTERFACE_HPP

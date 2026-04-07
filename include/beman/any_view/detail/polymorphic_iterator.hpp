// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_ITERATOR_HPP
#define BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_ITERATOR_HPP

#include <beman/any_view/detail/concepts.hpp>
#include <beman/any_view/detail/polymorphic.hpp>
#include <beman/any_view/detail/small_storage.hpp>
#include <beman/any_view/detail/unreachable.hpp>

#include <optional>

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

    [[nodiscard]] static constexpr type make(RefT ref) { return type{ref}; }
};

template <class RefT>
    requires std::is_lvalue_reference_v<RefT>
struct iter_cache<RefT> {
    using type = std::add_pointer_t<RefT>;

    [[nodiscard]] static constexpr type make(RefT ref) { return std::addressof(ref); }
};

template <class RefT>
using iter_cache_t = typename iter_cache<RefT>::type;

struct sentinel_compare_policy : unary_policy {
    template <not_adaptor T>
    static bool fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr bool fn(const IteratorAdaptorT& adaptor) {
        return adaptor.iterator == adaptor.sentinel;
    }
};

template <class RefT>
struct dereference_policy : unary_policy {
    template <not_adaptor T>
    static RefT fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr RefT fn(const IteratorAdaptorT& adaptor) {
        return *adaptor.iterator;
    }
};

template <class RefT>
struct cache_policy : unary_policy {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(const IteratorAdaptorT& adaptor) {
        if (dispatch<sentinel_compare_policy, IteratorAdaptorT>(adaptor)) {
            return {};
        }

        return iter_cache<RefT>::make(dispatch<dereference_policy<RefT>, IteratorAdaptorT>(adaptor));
    }
};

struct increment_policy : unary_policy {
    template <not_adaptor T>
    static void fn(T& self);

    template <adaptor IteratorAdaptorT>
    static constexpr void fn(IteratorAdaptorT& adaptor) {
        ++adaptor.iterator;
    }
};

template <class RefT>
struct next_policy : unary_policy {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(IteratorAdaptorT& adaptor) {
        dispatch<increment_policy, IteratorAdaptorT>(adaptor);
        return dispatch<cache_policy<RefT>, IteratorAdaptorT>(adaptor);
    }
};

struct decrement_policy : unary_policy {
    template <not_adaptor T>
    static void fn(T& self);

    template <adaptor IteratorAdaptorT>
    static constexpr void fn(IteratorAdaptorT& adaptor) {
        --adaptor.iterator;
    }
};

template <class RefT>
struct prev_policy : unary_policy {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(IteratorAdaptorT& adaptor) {
        dispatch<decrement_policy, IteratorAdaptorT>(adaptor);
        return dispatch<cache_policy<RefT>, IteratorAdaptorT>(adaptor);
    }
};

template <class RefT, class DiffT>
struct dereference_at_policy : unary_policy {
    template <not_adaptor T>
    static RefT fn(const T& self, DiffT n);

    template <adaptor IteratorAdaptorT>
    static constexpr RefT fn(const IteratorAdaptorT& adaptor, DiffT n) {
        return adaptor.iterator[n];
    }
};

template <class RvalueRefT, class DiffT>
struct iter_move_at_policy : unary_policy {
    template <not_adaptor T>
    static RvalueRefT fn(const T& self, DiffT n);

    template <adaptor IteratorAdaptorT>
    static constexpr RvalueRefT fn(const IteratorAdaptorT& adaptor, DiffT n) {
        return std::ranges::iter_move(adaptor.iterator + n);
    }
};

template <class RefT, class DiffT>
struct advance_policy : unary_policy {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(T& self, DiffT n);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(IteratorAdaptorT& adaptor, DiffT n) {
        adaptor.iterator += n;
        return dispatch<cache_policy<RefT>, IteratorAdaptorT>(adaptor);
    }
};

template <class RValueRefT>
struct iter_move_policy : unary_policy {
    template <not_adaptor T>
    static RValueRefT fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr RValueRefT fn(const IteratorAdaptorT& adaptor) {
        return std::ranges::iter_move(adaptor.iterator);
    }
};

struct equality_compare_policy : symmetric_binary_policy {
    [[nodiscard]] static constexpr bool default_value() noexcept { return false; }

    template <polymorphic PolyT>
    static bool fn(const PolyT& self, const PolyT& other);

    template <storage StorageT>
    static bool fn(const StorageT& self, const StorageT& other, const std::type_info& other_type);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr bool fn(const IteratorAdaptorT& adaptor, const IteratorAdaptorT& other) {
        return adaptor.iterator == other.iterator;
    }
};

struct three_way_compare_policy : symmetric_binary_policy {
    [[nodiscard]] static constexpr std::partial_ordering default_value() noexcept {
        return std::partial_ordering::unordered;
    }

    template <polymorphic PolyT>
    static std::partial_ordering fn(const PolyT& self, const PolyT& other);

    template <storage StorageT>
    static std::partial_ordering fn(const StorageT& self, const StorageT& other, const std::type_info& other_type);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr std::partial_ordering fn(const IteratorAdaptorT& adaptor,
                                                            const IteratorAdaptorT& other) {
        // std::__wrap_iter is not three-way comparable in Apple Clang
        if constexpr (std::three_way_comparable<decltype(adaptor.iterator)>) {
            return adaptor.iterator <=> other.iterator;
        } else {
            return adaptor.iterator < other.iterator    ? std::partial_ordering::less
                   : adaptor.iterator > other.iterator  ? std::partial_ordering::greater
                   : adaptor.iterator == other.iterator ? std::partial_ordering::equivalent
                                                        : std::partial_ordering::unordered;
        }
    }
};

template <class DiffT>
struct subtract_policy : symmetric_binary_policy {
    [[noreturn]] static DiffT default_value() { unreachable(); }

    template <polymorphic PolyT>
    static DiffT fn(const PolyT& self, const PolyT& other);

    template <storage StorageT>
    static DiffT fn(const StorageT& self, const StorageT& other, const std::type_info& other_type);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr DiffT fn(const IteratorAdaptorT& adaptor, const IteratorAdaptorT& other) {
        return adaptor.iterator - other.iterator;
    }
};

// inplace storage sufficient for two pointers
using iterator_storage = small_storage<2 * sizeof(void*)>;

template <class RefT, class RValueRefT>
using input_policy = inherit<move_policy<iterator_storage>,
                             destroy_policy<iterator_storage>,
                             dereference_policy<RefT>,
                             iter_move_policy<RValueRefT>,
                             increment_policy,
                             sentinel_compare_policy>;

template <class RefT>
inline constexpr bool has_cache_v = not std::is_same_v<iter_cache_t<RefT>, no_cache>;

template <class RefT, class RValueRefT>
using forward_policy =
    inherit<input_policy<RefT, RValueRefT>,
            copy_policy<iterator_storage>,
            std::conditional_t<has_cache_v<RefT>, inherit<cache_policy<RefT>, next_policy<RefT>>, inherit<>>,
            type_policy,
            equality_compare_policy>;

template <class RefT, class RValueRefT>
using bidirectional_policy = inherit<forward_policy<RefT, RValueRefT>,
                                     std::conditional_t<has_cache_v<RefT>, prev_policy<RefT>, decrement_policy>>;

template <class RefT, class RValueRefT, class DiffT>
using random_access_policy =
    inherit<bidirectional_policy<RefT, RValueRefT>,
            std::conditional_t<has_cache_v<RefT>,
                               advance_policy<RefT, DiffT>,
                               inherit<dereference_at_policy<RefT, DiffT>, iter_move_at_policy<RValueRefT, DiffT>>>,
            three_way_compare_policy,
            subtract_policy<DiffT>>;

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
[[nodiscard]] consteval auto get_iterator_policy() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, random_access>) {
        return random_access_policy<RefT, RValueRefT, DiffT>{};
    } else if constexpr (flag_is_set<OptsV, bidirectional>) {
        return bidirectional_policy<RefT, RValueRefT>{};
    } else if constexpr (flag_is_set<OptsV, forward>) {
        return forward_policy<RefT, RValueRefT>{};
    } else if constexpr (flag_is_set<OptsV, input>) {
        return input_policy<RefT, RValueRefT>{};
    }
}

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using iterator_policy = decltype(get_iterator_policy<RefT, RValueRefT, DiffT, OptsV>());

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using polymorphic_iterator = basic_polymorphic<iterator_storage, iterator_policy<RefT, RValueRefT, DiffT, OptsV>>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_ITERATOR_HPP

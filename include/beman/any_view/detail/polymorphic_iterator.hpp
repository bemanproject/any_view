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

struct sentinel_compare_t : unary_protocol {
    template <not_adaptor T>
    static bool fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr bool fn(const IteratorAdaptorT& adaptor) {
        return adaptor.iterator == adaptor.sentinel;
    }
};

template <class RefT>
struct dereference_t : unary_protocol {
    template <not_adaptor T>
    static RefT fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr RefT fn(const IteratorAdaptorT& adaptor) {
        return *adaptor.iterator;
    }
};

template <class RefT>
struct cache_t : unary_protocol {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(const IteratorAdaptorT& adaptor) {
        if (adaptor.iterator == adaptor.sentinel) {
            return {};
        }

        return iter_cache<RefT>::make(*adaptor.iterator);
    }
};

struct increment_t : unary_protocol {
    template <not_adaptor T>
    static void fn(T& self);

    template <adaptor IteratorAdaptorT>
    static constexpr void fn(IteratorAdaptorT& adaptor) {
        ++adaptor.iterator;
    }
};

template <class RefT>
struct next_t : unary_protocol {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(IteratorAdaptorT& adaptor) {
        ++adaptor.iterator;
        return dispatch<cache_t<RefT>, IteratorAdaptorT>(adaptor);
    }
};

struct decrement_t : unary_protocol {
    template <not_adaptor T>
    static void fn(T& self);

    template <adaptor IteratorAdaptorT>
    static constexpr void fn(IteratorAdaptorT& adaptor) {
        --adaptor.iterator;
    }
};

template <class RefT>
struct prev_t : unary_protocol {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(IteratorAdaptorT& adaptor) {
        --adaptor.iterator;
        return dispatch<cache_t<RefT>, IteratorAdaptorT>(adaptor);
    }
};

template <class RefT, class DiffT>
struct dereference_at_t : unary_protocol {
    template <not_adaptor T>
    static RefT fn(const T& self, DiffT n);

    template <adaptor IteratorAdaptorT>
    static constexpr RefT fn(const IteratorAdaptorT& adaptor, DiffT n) {
        return adaptor.iterator[n];
    }
};

template <class RvalueRefT, class DiffT>
struct iter_move_at_t : unary_protocol {
    template <not_adaptor T>
    static RvalueRefT fn(const T& self, DiffT n);

    template <adaptor IteratorAdaptorT>
    static constexpr RvalueRefT fn(const IteratorAdaptorT& adaptor, DiffT n) {
        return std::ranges::iter_move(adaptor.iterator + n);
    }
};

template <class RefT, class DiffT>
struct advance_t : unary_protocol {
    template <not_adaptor T>
    static iter_cache_t<RefT> fn(T& self, DiffT n);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr iter_cache_t<RefT> fn(IteratorAdaptorT& adaptor, DiffT n) {
        adaptor.iterator += n;
        return dispatch<cache_t<RefT>, IteratorAdaptorT>(adaptor);
    }
};

template <class RValueRefT>
struct iter_move_t : unary_protocol {
    template <not_adaptor T>
    static RValueRefT fn(const T& self);

    template <adaptor IteratorAdaptorT>
    [[nodiscard]] static constexpr RValueRefT fn(const IteratorAdaptorT& adaptor) {
        return std::ranges::iter_move(adaptor.iterator);
    }
};

template <class RefT>
struct sync_t : unary_protocol {
    template <not_adaptor T>
    static void fn(T& self, iter_cache_t<RefT> cache);

    template <adaptor IteratorAdaptorT>
    static constexpr void fn(IteratorAdaptorT& adaptor, iter_cache_t<RefT> cache) {
        const auto n = cache - std::to_address(adaptor.iterator);
        adaptor.iterator += n;
    }
};

struct equality_compare_t : symmetric_binary_protocol {
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

struct three_way_compare_t : symmetric_binary_protocol {
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
        return std::compare_partial_order_fallback(adaptor.iterator, other.iterator);
    }
};

template <class DiffT>
struct subtract_t : symmetric_binary_protocol {
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
struct input_protocol : inherit<move_t<iterator_storage>,
                                destroy_t<iterator_storage>,
                                dereference_t<RefT>,
                                iter_move_t<RValueRefT>,
                                increment_t,
                                sentinel_compare_t> {};

template <class RefT>
concept has_cache = not std::same_as<iter_cache_t<RefT>, no_cache>;

template <class RefT>
struct forward_cache_protocol : inherit<> {};

template <has_cache RefT>
struct forward_cache_protocol<RefT> : inherit<cache_t<RefT>, next_t<RefT>> {};

template <class RefT, class RValueRefT>
struct forward_protocol : inherit<input_protocol<RefT, RValueRefT>,
                                  copy_t<iterator_storage>,
                                  forward_cache_protocol<RefT>,
                                  type_t,
                                  equality_compare_t> {};

template <class RefT>
struct bidirectional_cache_protocol : inherit<decrement_t> {};

template <has_cache RefT>
struct bidirectional_cache_protocol<RefT> : inherit<prev_t<RefT>> {};

template <class RefT, class RValueRefT>
struct bidirectional_protocol : inherit<forward_protocol<RefT, RValueRefT>, bidirectional_cache_protocol<RefT>> {};

template <class RefT, class RValueRefT, class DiffT>
struct random_access_cache_protocol : inherit<dereference_at_t<RefT, DiffT>, iter_move_at_t<RValueRefT, DiffT>> {};

template <has_cache RefT, class RValueRefT, class DiffT>
struct random_access_cache_protocol<RefT, RValueRefT, DiffT> : inherit<advance_t<RefT, DiffT>> {};

template <class RefT, class RValueRefT, class DiffT>
struct random_access_protocol : inherit<bidirectional_protocol<RefT, RValueRefT>,
                                        random_access_cache_protocol<RefT, RValueRefT, DiffT>,
                                        three_way_compare_t,
                                        subtract_t<DiffT>> {};

template <class RefT, class RValueRefT, class DiffT>
struct contiguous_protocol : inherit<random_access_protocol<RefT, RValueRefT, DiffT>, sync_t<RefT>> {};

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
[[nodiscard]] consteval auto get_iterator_protocol() {
    using enum any_view_options;

    if constexpr (flag_is_set<OptsV, contiguous>) {
        return contiguous_protocol<RefT, RValueRefT, DiffT>{};
    } else if constexpr (flag_is_set<OptsV, random_access>) {
        return random_access_protocol<RefT, RValueRefT, DiffT>{};
    } else if constexpr (flag_is_set<OptsV, bidirectional>) {
        return bidirectional_protocol<RefT, RValueRefT>{};
    } else if constexpr (flag_is_set<OptsV, forward>) {
        return forward_protocol<RefT, RValueRefT>{};
    } else if constexpr (flag_is_set<OptsV, input>) {
        return input_protocol<RefT, RValueRefT>{};
    }
}

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using iterator_protocol = decltype(get_iterator_protocol<RefT, RValueRefT, DiffT, OptsV>());

template <class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
using polymorphic_iterator = basic_polymorphic<iterator_storage, iterator_protocol<RefT, RValueRefT, DiffT, OptsV>>;

} // namespace beman::any_view::detail

#endif // BEMAN_ANY_VIEW_DETAIL_POLYMORPHIC_ITERATOR_HPP

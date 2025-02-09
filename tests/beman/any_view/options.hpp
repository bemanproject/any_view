// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_TEST_OPTIONS_HPP
#define BEMAN_ANY_VIEW_TEST_OPTIONS_HPP

#include <beman/any_view/config.hpp>

#if BEMAN_ANY_VIEW_USE_ENUM()

#include <beman/any_view/any_view_options.hpp>

using options = beman::any_view::any_view_options;

#if BEMAN_ANY_VIEW_USE_COPYABLE()
inline constexpr auto copyable_options  = options::copyable;
inline constexpr auto move_only_options = options{};
#elif BEMAN_ANY_VIEW_USE_MOVE_ONLY()
inline constexpr auto copyable_options  = options{};
inline constexpr auto move_only_options = options::move_only;
#endif

inline constexpr auto input_options         = move_only_options | options::input;
inline constexpr auto forward_options       = move_only_options | options::forward;
inline constexpr auto bidirectional_options = move_only_options | options::bidirectional;
inline constexpr auto random_access_options = move_only_options | options::random_access;
inline constexpr auto contiguous_options    = move_only_options | options::contiguous;
inline constexpr auto sized_options         = move_only_options | options::sized;
inline constexpr auto borrowed_options      = move_only_options | options::borrowed;

#elif BEMAN_ANY_VIEW_USE_TRAITS()

#include <iterator>

struct copyable_options {
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    static constexpr bool copyable = true;
#endif
};

struct move_only_options {
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    static constexpr bool move_only = true;
#endif
};

template <class IterConceptT>
struct iterator_options : move_only_options {
    using iterator_concept = IterConceptT;
};

using input_options         = iterator_options<std::input_iterator_tag>;
using forward_options       = iterator_options<std::forward_iterator_tag>;
using bidirectional_options = iterator_options<std::bidirectional_iterator_tag>;
using random_access_options = iterator_options<std::random_access_iterator_tag>;
using contiguous_options    = iterator_options<std::contiguous_iterator_tag>;

struct sized_options : move_only_options {
    static constexpr bool sized = true;
};

struct borrowed_options : move_only_options {
    static constexpr bool borrowed = true;
};

template <class... OptionsT>
struct inherit : OptionsT... {};

#elif BEMAN_ANY_VIEW_USE_NAMED()

#include <beman/any_view/any_view_options.hpp>

using beman::any_view::any_view_options;
using beman::any_view::type;

inline constexpr auto default_reference_type = type<int&>;

inline constexpr any_view_options copyable_options{
    .reference_type = default_reference_type,
#if BEMAN_ANY_VIEW_USE_COPYABLE()
    .copyable = true,
#endif
};

inline constexpr any_view_options move_only_options{
    .reference_type = default_reference_type,
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    .move_only = true,
#endif
};

template <class IterConceptT>
inline constexpr any_view_options iterator_options{
    .reference_type   = default_reference_type,
    .iterator_concept = type<IterConceptT>,
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    .move_only = true,
#endif
};

inline constexpr auto input_options         = iterator_options<std::input_iterator_tag>;
inline constexpr auto forward_options       = iterator_options<std::forward_iterator_tag>;
inline constexpr auto bidirectional_options = iterator_options<std::bidirectional_iterator_tag>;
inline constexpr auto random_access_options = iterator_options<std::random_access_iterator_tag>;
inline constexpr auto contiguous_options    = iterator_options<std::contiguous_iterator_tag>;

inline constexpr any_view_options sized_options{
    .reference_type = default_reference_type,
    .sized          = true,
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    .move_only = true,
#endif
};

inline constexpr any_view_options borrowed_options{
    .reference_type = default_reference_type,
#if BEMAN_ANY_VIEW_USE_MOVE_ONLY()
    .move_only = true,
#endif
    .borrowed = true,
};

#endif

#endif // BEMAN_ANY_VIEW_TEST_OPTIONS_HPP

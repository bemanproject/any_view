// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_UNIQUE_ADDRESS_HPP
#define BEMAN_ANY_VIEW_DETAIL_UNIQUE_ADDRESS_HPP

#ifdef __has_include
#if __has_include(<beman/any_view/config.hpp>)
#include <beman/any_view/config.hpp>
#endif // __has_include(<beman/any_view/config.hpp>)
#endif // __has_include

// if the config.hpp file does not exist because this is in godbolt
// or another context where cmake is not run, default to the correct
// answer.
#ifndef BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS
#if _MSC_VER
#define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS() [[msvc::no_unique_address]]
#else
#define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS() [[no_unique_address]]
#endif // _MSC_VER
#endif // BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS

#endif // BEMAN_ANY_VIEW_DETAIL_UNIQUE_ADDRESS_HPP

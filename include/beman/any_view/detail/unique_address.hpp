// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_ANY_VIEW_DETAIL_UNIQUE_ADDRESS_HPP
#define BEMAN_ANY_VIEW_DETAIL_UNIQUE_ADDRESS_HPP

#if !defined(__has_include) || __has_include(<beman/any_view/config.hpp>)
#include <beman/any_view/config.hpp>
#else
  // if the config.hpp file does not exist because this is in godbolt
  // or another context where cmake is not run, default to the correct
  // answer.
  #if (_MSC_VER)
    #define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS() [[msvc::no_unique_address]]
  #else 
    #define BEMAN_ANY_VIEW_NO_UNIQUE_ADDRESS() [[no_unique_address]]
  #endif
#endif

#endif

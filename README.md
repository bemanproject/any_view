<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# beman.any_view: A generalized type-erased view with customizable properties

![Library Status](https://github.com/bemanproject/beman/blob/c6997986557ec6dda98acbdf502082cdf7335526/images/badges/beman_badge-beman_library_under_development.svg)
![Continuous Integration Tests](https://github.com/bemanproject/any_view/actions/workflows/ci_tests.yml/badge.svg)
![Lint Check (pre-commit)](https://github.com/bemanproject/any_view/actions/workflows/pre-commit.yml/badge.svg)

**Implements**: `std::ranges::any_view` proposed in [any_view (P3411R1)](https://wg21.link/P3411R1).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_LIBRARY_MATURITY_MODEL.md#under-development-and-not-yet-ready-for-production-use)

## Usage

`std::ranges::any_view` is a class template that provides a type-erased interface for `std::ranges::view`. It may additionally model other concepts like `std::ranges::contiguous_range`, `std::ranges::sized_range`, `std::ranges::borrowed_range`, and `std::copyable` depending on the instantiation.

## Integrate beman.any_view into your project

<details>
<summary>Use <code>beman.any_view</code> directly from CMake</summary>

For CMake based projects, you can include it as a dependency using the `FetchContent` module:

```cmake
include(FetchContent)

FetchContent_Declare(
  beman.any_view
  GIT_REPOSITORY https://github.com/bemanproject/any_view.git
  GIT_TAG main
  EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(beman.any_view)
```

You will also need to add `beman::any_view` to the link libraries of any targets that include `beman/any_view/*.hpp` in their source or header files:

```cmake
target_link_libraries(yourlib PUBLIC beman::any_view)
```

</details>

## Contributing

Please do! Issues and pull requests are appreciated.

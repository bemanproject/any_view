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

```cpp
#include <beman/any_view/any_view.hpp>

#include <ranges>
#include <vector>

namespace bav = beman::any_view;
using opt = bav::any_view_options;

template <class T>
using proxy_any_view = bav::any_view<T, opt::input, T>;

constexpr auto sum(proxy_any_view<proxy_any_view<int>> views) {
    auto result = 0;

    for (auto view : views) {
        for (const auto value : view) {
            result += value;
        }
    }

    return result;
}

static_assert(10 == sum(std::vector{std::vector{1, 2}, std::vector{3, 4}}));

constexpr auto iota(int n) { return std::views::iota(1) | std::views::take(n); };

static_assert(35 == sum(iota(5) | std::views::transform(iota)));

static_assert(22 == sum(std::vector{iota(1), iota(3), iota(5)}));
```

Full code can be found in [tests/beman/any_view/constexpr.test.cpp](tests/beman/any_view/constexpr.test.cpp).

`std::ranges::any_view` is a class template that provides a type-erased interface for `std::ranges::view`.
It may additionally model other concepts like `std::ranges::contiguous_range`, `std::ranges::sized_range`,
`std::ranges::borrowed_range`, and `std::copyable` depending on the instantiation.

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

You will also need to add `beman::any_view` to the link libraries of any targets that include `beman/any_view/*.hpp` in
their source or header files:

```cmake
target_link_libraries(yourlib PUBLIC beman::any_view)
```

</details>

## Reference

Synopsis:

```cpp
namespace beman::any_view {

enum class any_view_options {
    input         = 0b0000000,
    forward       = 0b0000001,
    bidirectional = 0b0000011,
    random_access = 0b0000111,
    contiguous    = 0b0001111,
    sized         = 0b0010000,
    borrowed      = 0b0100000,
    copyable      = 0b1000000,
};

constexpr auto operator|(any_view_options l, any_view_options r) noexcept -> any_view_options;
constexpr auto operator&(any_view_options l, any_view_options r) noexcept -> any_view_options;
constexpr auto operator^(any_view_options l, any_view_options r) noexcept -> any_view_options;

constexpr auto operator~(any_view_options o) noexcept -> any_view_options;

constexpr auto operator|=(any_view_options& l, any_view_options r) noexcept -> any_view_options&;
constexpr auto operator&=(any_view_options& l, any_view_options r) noexcept -> any_view_options&;
constexpr auto operator^=(any_view_options& l, any_view_options r) noexcept -> any_view_options&;

template <class ElementT,
          any_view_options OptionsV = any_view_options::input,
          class RefT                = ElementT&,
          class RValueRefT          = /*as-rvalue-t*/<RefT>,
          class DiffT               = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> {
    class iterator; // exposition-only
    class sentinel; // exposition-only

    using size_type = /*make-unsigned-like-t*/<DiffT>; // exposition-only

    static constexpr bool copyable = // exposition-only
        (OptionsV & any_view_options::copyable) == any_view_options::copyable;

    static constexpr bool sized = // exposition-only
        (OptionsV & any_view_options::sized) == any_view_options::sized;

  public:
    template </*viewable-range-compatible-with*/<any_view> RangeT>
    constexpr any_view(RangeT&& range);

    constexpr any_view(const any_view&) requires copyable;

    constexpr any_view(any_view&&) noexcept;

    constexpr auto operator=(const any_view&) -> any_view& requires copyable;

    constexpr auto operator=(any_view&&) noexcept -> any_view&;

    constexpr auto begin() -> iterator;

    constexpr auto end() -> sentinel;

    constexpr auto size() const -> size_type requires sized;
};

} // namespace beman::any_view

template <class ElementT, auto OptionsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool std::ranges::enable_borrowed_range<
    beman::any_view::any_view<ElementT, OptionsV, RefT, RValueRefT, DiffT>> =
        (OptionsV & beman::any_view::any_view_options::borrowed) == beman::any_view::any_view_options::borrowed;
```

## Building

There are workflows available in CMake presets such as `gcc-debug`:

```bash
cmake --workflow --preset gcc-debug
```

Alternatively you can manually configure, build, and test with CMake and CTest:

```bash
cmake -B build
cmake --build build
ctest --test-dir build
```

Possible output:

```text
Executing workflow step 1 of 3: configure preset "gcc-debug"

Preset CMake variables:

  BEMAN_BUILDSYS_SANITIZER="MaxSan"
  CMAKE_BUILD_TYPE="Debug"
  CMAKE_EXPORT_COMPILE_COMMANDS:BOOL="TRUE"
  CMAKE_TOOLCHAIN_FILE="cmake/gnu-toolchain.cmake"

-- The CXX compiler identification is GNU 15.0.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/g++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- The C compiler identification is GNU 15.0.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/gcc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Found Python3: /usr/bin/python3.12 (found version "3.12.7") found components: Interpreter
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE
-- Configuring done (2.7s)
-- Generating done (0.0s)
-- Build files have been written to: /home/patrick/projects/any_view/build/gcc-debug

Executing workflow step 2 of 3: build preset "gcc-debug"

[12/12] Linking CXX executable tests/beman/any_view/beman.any_view.tests.constexpr

Executing workflow step 3 of 3: test preset "gcc-debug"

Test project /home/patrick/projects/any_view/build/gcc-debug
      Start  1: ConceptsTest.iterator_concept
 1/21 Test  #1: ConceptsTest.iterator_concept ...................   Passed    0.01 sec
      Start  2: ConceptsTest.sized_concept
 2/21 Test  #2: ConceptsTest.sized_concept ......................   Passed    0.01 sec
      Start  3: ConceptsTest.borrowed_concept
 3/21 Test  #3: ConceptsTest.borrowed_concept ...................   Passed    0.01 sec
      Start  4: ConceptsTest.copyable_concept
 4/21 Test  #4: ConceptsTest.copyable_concept ...................   Passed    0.01 sec
      Start  5: ConstexprTest.sum_vector_of_vector
 5/21 Test  #5: ConstexprTest.sum_vector_of_vector ..............   Passed    0.01 sec
      Start  6: ConstexprTest.sum_transform_view_of_iota_view
 6/21 Test  #6: ConstexprTest.sum_transform_view_of_iota_view ...   Passed    0.01 sec
      Start  7: ConstexprTest.sum_vector_of_iota_view
 7/21 Test  #7: ConstexprTest.sum_vector_of_iota_view ...........   Passed    0.01 sec
      Start  8: ConstexprTest.sort_vector
 8/21 Test  #8: ConstexprTest.sort_vector .......................   Passed    0.01 sec
      Start  9: SfinaeTest.istream_view
 9/21 Test  #9: SfinaeTest.istream_view .........................   Passed    0.01 sec
      Start 10: SfinaeTest.forward_list
10/21 Test #10: SfinaeTest.forward_list .........................   Passed    0.01 sec
      Start 11: SfinaeTest.list
11/21 Test #11: SfinaeTest.list .................................   Passed    0.01 sec
      Start 12: SfinaeTest.deque
12/21 Test #12: SfinaeTest.deque ................................   Passed    0.01 sec
      Start 13: SfinaeTest.vector
13/21 Test #13: SfinaeTest.vector ...............................   Passed    0.01 sec
      Start 14: SfinaeTest.vector_of_bool
14/21 Test #14: SfinaeTest.vector_of_bool .......................   Passed    0.01 sec
      Start 15: SfinaeTest.span
15/21 Test #15: SfinaeTest.span .................................   Passed    0.01 sec
      Start 16: TypeTraitsTest.value_type
16/21 Test #16: TypeTraitsTest.value_type .......................   Passed    0.01 sec
      Start 17: TypeTraitsTest.reference_type
17/21 Test #17: TypeTraitsTest.reference_type ...................   Passed    0.01 sec
      Start 18: TypeTraitsTest.rvalue_reference_type
18/21 Test #18: TypeTraitsTest.rvalue_reference_type ............   Passed    0.01 sec
      Start 19: TypeTraitsTest.difference_type
19/21 Test #19: TypeTraitsTest.difference_type ..................   Passed    0.01 sec
      Start 20: TypeTraitsTest.size_type
20/21 Test #20: TypeTraitsTest.size_type ........................   Passed    0.01 sec
      Start 21: TypeTraitsTest.borrowed_iterator_type
21/21 Test #21: TypeTraitsTest.borrowed_iterator_type ...........   Passed    0.01 sec

100% tests passed, 0 tests failed out of 21

Total Test time (real) =   0.15 sec
```

## Contributing

Please do! Issues and pull requests are appreciated.

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

# beman.any_view: A generalized type-erased view with customizable properties

![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg)
![Continuous Integration Tests](https://github.com/bemanproject/any_view/actions/workflows/ci_tests.yml/badge.svg)
![Lint Check (pre-commit)](https://github.com/bemanproject/any_view/actions/workflows/pre-commit.yml/badge.svg)
![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg)

**Implements**: `std::ranges::any_view` proposed in [any_view (P3411)](https://wg21.link/p3411).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_LIBRARY_MATURITY_MODEL.md#under-development-and-not-yet-ready-for-production-use)

**Featured**: [C++Now 2025](https://schedule.cppnow.org/wp-content/uploads/2025/03/A-View-for-Any-Occasion.pdf)

## Motivation

`any_view` equips library APIs with the ability to declare function parameters and return types as views, while
de-coupling the interface from an implementation that can fully leverage the power of C++20 ranges. By using
type-erased views, the library can be compiled separately.

As a return type, `any_view` grants a library author the freedom to change how the function is implemented to return
the view without modifying its interface. As a parameter type, it allows users to call the library function with any
range as an argument that can be consumed directly, rather than copying it into some bespoke, library-specific type.

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
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(beman.any_view)
```

You will also need to add `beman::any_view` to the link libraries of any targets that include `beman/any_view/*.hpp` in
their source or header files:

```cmake
target_link_libraries(yourlib PUBLIC beman::any_view)
```

</details>

## Reference

<details>
<summary>Synopsis</summary>

```cpp
namespace beman::any_view {

// [range.any]
enum class any_view_options {
    input         = 0b00000001,
    forward       = 0b00000011,
    bidirectional = 0b00000111,
    random_access = 0b00001111,
    contiguous    = 0b00011111,
    sized         = 0b00100000,
    borrowed      = 0b01000000,
    copyable      = 0b10000000,
};

constexpr any_view_options operator|(any_view_options, any_view_options) noexcept;
constexpr any_view_options operator&(any_view_options, any_view_options) noexcept;

template <class T>
struct /*rvalue-ref*/ {
    using type = T;
};

template <class T>
struct /*rvalue-ref*/<T&> {
    using type = T&&;
};

template <class T>
using /*rvalue-ref-t*/ = /*rvalue-ref*/<T>::type;

template <class RangeT, class AnyViewT>
concept ext_any_compatible_viewable_range = /* ... */;

template <class ElementT,
          any_view_options OptsV = any_view_options::input,
          class RefT             = ElementT&,
          class RValueRefT       = /*as-rvalue-t*/<RefT>,
          class DiffT            = std::ptrdiff_t>
class any_view : public std::ranges::view_interface<any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> {
    class iterator; // exposition-only
    class sentinel; // exposition-only

  public:
    // [range.any.ctor]
    template <class RangeT>
    constexpr any_view(RangeT&& range);
    constexpr any_view(const any_view&);
    constexpr any_view(any_view&&) noexcept;

    constexpr any_view& operator=(const any_view&);
    constexpr any_view& operator=(any_view&&) noexcept;

    constexpr ~any_view();

    // [range.any.access]
    constexpr iterator begin();

    constexpr sentinel end();

    constexpr std::make_unsigned_t<DiffT> size() const;

    // [range.any.swap]
    constexpr void swap(any_view&) noexcept;

    constexpr friend void swap(any_view&, any_view&) noexcept;
};

} // namespace beman::any_view

template <class ElementT, beman::any_view::any_view_options OptsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool std::ranges::enable_borrowed_range<
    beman::any_view::any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> =
        bool(OptsV & beman::any_view::any_view_options::borrowed);
```

</details>

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

<details>
<summary>Possible output</summary>

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

</details>

## Performance

Various benchmarks are available to build and run using the CMake target `beman.any_view.benchmarks`.

<details>
<summary>Example output comparing <code>reserved</code> to <code>fused</code> for <code>beman.any_view.benchmarks.all</code></summary>

```text
+ cmake --build build --config Release --target beman.any_view.benchmarks.all
[27/27] Linking CXX executable tests/beman/any_view/beman.any_view.benchmarks.all
+ build/_deps/benchmark-src/tools/compare.py benchmarksfiltered build/tests/beman/any_view/beman.any_view.benchmarks.all reserved build/tests/beman/any_view/beman.any_view.benchmarks.all fused
RUNNING: build/tests/beman/any_view/beman.any_view.benchmarks.all --benchmark_filter=reserved
Run on (32 X 2419.2 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 0.25, 0.17, 0.11
-----------------------------------------------------------------
Benchmark                       Time             CPU   Iterations
-----------------------------------------------------------------
BM_all_reserved/1024         1783 ns         1783 ns       389936
BM_all_reserved/2048         3839 ns         3839 ns       191729
BM_all_reserved/4096         7067 ns         7067 ns        89826
BM_all_reserved/8192        14756 ns        14756 ns        48245
BM_all_reserved/16384       30317 ns        30317 ns        23191
BM_all_reserved/32768       73651 ns        73650 ns         9626
BM_all_reserved/65536      215342 ns       215341 ns         3413
BM_all_reserved/131072     424794 ns       424793 ns         1654
BM_all_reserved/262144     887833 ns       887828 ns          789
RUNNING: build/tests/beman/any_view/beman.any_view.benchmarks.all --benchmark_filter=fused
Run on (32 X 2419.2 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 0.31, 0.19, 0.11
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
BM_all_fused/1024         1660 ns         1660 ns       437469
BM_all_fused/2048         3219 ns         3219 ns       210096
BM_all_fused/4096         7100 ns         7100 ns       100823
BM_all_fused/8192        15088 ns        15088 ns        45117
BM_all_fused/16384       33719 ns        33719 ns        21827
BM_all_fused/32768       72098 ns        72097 ns         9582
BM_all_fused/65536      150699 ns       150698 ns         4629
BM_all_fused/131072     316431 ns       316430 ns         2234
BM_all_fused/262144     583499 ns       583500 ns         1091
Comparing reserved (from build/tests/beman/any_view/beman.any_view.benchmarks.all) to fused (from build/tests/beman/any_view/beman.any_view.benchmarks.all)
Benchmark                                            Time             CPU      Time Old      Time New       CPU Old       CPU New
---------------------------------------------------------------------------------------------------------------------------------
BM_all_[reserved vs. fused]/1024                  -0.0689         -0.0689          1783          1660          1783          1660
BM_all_[reserved vs. fused]/2048                  -0.1615         -0.1615          3839          3219          3839          3219
BM_all_[reserved vs. fused]/4096                  +0.0047         +0.0047          7067          7100          7067          7100
BM_all_[reserved vs. fused]/8192                  +0.0225         +0.0225         14756         15088         14756         15088
BM_all_[reserved vs. fused]/16384                 +0.1122         +0.1122         30317         33719         30317         33719
BM_all_[reserved vs. fused]/32768                 -0.0211         -0.0211         73651         72098         73650         72097
BM_all_[reserved vs. fused]/65536                 -0.3002         -0.3002        215342        150699        215341        150698
BM_all_[reserved vs. fused]/131072                -0.2551         -0.2551        424794        316431        424793        316430
BM_all_[reserved vs. fused]/262144                -0.3428         -0.3428        887833        583499        887828        583500
OVERALL_GEOMEAN                                   -0.1255         -0.1255
```

</details>

## Contributing

Please do! Issues and pull requests are appreciated.

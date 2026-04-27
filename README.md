# beman.any_view: A generalized type-erased view with customizable properties

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/any_view/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/any_view/actions/workflows/pre-commit-check.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/bemanproject/any_view/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/any_view?branch=main) ![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg) [![Compiler Explorer Example](https://img.shields.io/badge/Try%20it%20on%20Compiler%20Explorer-grey?logo=compilerexplorer&logoColor=67c52a)](https://godbolt.org/z/Y5hefMj65)

**Implements**: `std::ranges::any_view` proposed in [any_view (P3411)](https://wg21.link/p3411).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

**Featured**: [C++Now 2025](https://schedule.cppnow.org/wp-content/uploads/2025/03/A-View-for-Any-Occasion.pdf)

## Motivation

`any_view` equips library APIs with the ability to declare function parameters and return types as views, while
de-coupling the interface from an implementation that can fully leverage the power of C++20 ranges. By using
type-erased views, the library can be compiled separately.

As a return type, `any_view` grants a library author the freedom to change how the function is implemented to return
the view without modifying its interface. As a parameter type, it allows users to call the library function with any
range as an argument that can be consumed directly, rather than copying it into some bespoke, library-specific type.

## License

`beman.any_view` is licensed under the Apache License v2.0 with LLVM Exceptions.

## Usage

### Basic Example: Library Function with Type Erasure

The main use case for `any_view` is allowing library functions to accept and return ranges without exposing
implementation details.

```cpp
#include <beman/any_view/any_view.hpp>

#include <print>
#include <ranges>
#include <vector>

namespace bav = beman::any_view;
using opt = bav::any_view_options;

// Computes average for any sized forward range of integers
// Users don't need to provide a specific container type
auto average(bav::any_view<int, opt::forward | opt::sized, int> values) {
    int sum = 0;
    for (int value : values) {
        sum += value;
    }
    return sum / values.size();
}

int main() {
    // Users can pass any compatible range
    std::vector<int> vec{1, 2, 3, 4, 5};
    // Outputs 3
    std::println("{}", average(vec));

    // Works with generated ranges too
    std::println("{}", average(std::views::iota(1, 6))); // Also outputs 3
}
```

### Return Type Encapsulation

`any_view` allows a library to hide the complexity of its implementation from users:

```cpp
namespace bav = beman::any_view;

// Library can return this without exposing complex range composition
// Users just get an opaque input view of strings
auto get_configuration_keys_starting_with(std::string_view prefix)
    -> bav::any_view<const std::string> {
    // get_configuration() could return something like std::map<std::string, int>
    return get_configuration()
      | std::views::keys
      | std::views::filter([=](std::string_view key) {
            return key.starts_with(prefix);
        });
}

// Users can consume it naturally
for (const auto& key : get_configuration_keys_starting_with("max")) {
    std::println("{}", key);
}
```

### Compile-Time Range Processing

`any_view` also supports constant evaluation for compile-time range operations:

```cpp
constexpr auto sum(bav::any_view<const int> values) -> int {
    int result = 0;
    for (int value : values) {
        result += value;
    }
    return result;
}

static_assert(15 == sum(std::vector{1, 2, 3, 4, 5}));
```

More complete examples can be found in [examples](examples/) and [tests](tests/beman/any_view/).

### What `any_view` Provides

`std::ranges::any_view` is a class template that type-erases a range and models `std::ranges::view`.
It decouples library interfaces from implementation, allowing users to pass any compatible range without modification.
The type may additionally model concepts like `std::ranges::contiguous_range`, `std::ranges::sized_range`,
`std::ranges::borrowed_range`, and `std::copyable` depending on configuration.

Full runnable examples can be found in [`examples/`](examples/).

## Dependencies

### Build Environment

This project requires at least the following to build:

* A C++ compiler that conforms to the C++20 standard or greater
* CMake 3.30 or later
* (Test Only) GoogleTest
* (Benchmark Only) Google Benchmark

You can disable building tests by setting CMake option `BEMAN_ANY_VIEW_BUILD_TESTS` to
`OFF` when configuring the project.

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 15-13   | C++26-C++20   | libstdc++         |
| GCC        | 12      | C++23, C++20  | libstdc++         |
| Clang      | 22-19   | C++26-C++20   | libstdc++, libc++ |
| Clang      | 18      | C++26-C++20   | libc++            |
| Clang      | 18      | C++23, C++20  | libstdc++         |
| AppleClang | latest  | C++26-C++20   | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |

## Development

See the [Contributing Guidelines](CONTRIBUTING.md).

## Integrate beman.any_view into your project

### Build

You can build any_view using a CMake workflow preset:

```bash
cmake --workflow --preset gcc-release
```

To list available workflow presets, you can invoke:

```bash
cmake --list-presets=workflow
```

For details on building beman.any_view without using a CMake preset, refer to the
[Contributing Guidelines](CONTRIBUTING.md).

### Installation

To install beman.any_view globally after building with the `gcc-release` preset, you can
run:

```bash
sudo cmake --install build/gcc-release
```

Alternatively, to install to a prefix, for example `/opt/beman`, you can run:

```bash
sudo cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure:

```txt
/opt/beman
├── include
│   └── beman
│       └── any_view
│           ├── any_view.hpp
│           └── ...
└── lib
    └── cmake
        └── beman.any_view
            ├── beman.any_view-config-version.cmake
            ├── beman.any_view-config.cmake
            └── beman.any_view-targets.cmake
```

### CMake Configuration

If you installed beman.any_view to a prefix, you can specify that prefix to your CMake
project using `CMAKE_PREFIX_PATH`; for example, `-DCMAKE_PREFIX_PATH=/opt/beman`.

You need to bring in the `beman.any_view` package to define the `beman::any_view` CMake
target:

```cmake
find_package(beman.any_view REQUIRED)
```

You will then need to add `beman::any_view` to the link libraries of any libraries or
executables that include `beman.any_view` headers.

```cmake
target_link_libraries(yourlib PUBLIC beman::any_view)
```

### Using beman.any_view

To use `beman.any_view` in your C++ project,
include an appropriate `beman.any_view` header from your source code.

```c++
#include <beman/any_view/any_view.hpp>
```

> [!NOTE]
>
> `beman.any_view` headers are to be included with the `beman/any_view/` prefix.
> Altering include search paths to spell the include target another way (e.g.
> `#include <any_view.hpp>`) is unsupported.

## Reference

<details>
<summary>Synopsis</summary>

```cpp
namespace beman::any_view {

// [range.any]
enum class any_view_options {
    input               = 0b000000001,
    forward             = 0b000000011,
    bidirectional       = 0b000000111,
    random_access       = 0b000001111,
    contiguous          = 0b000011111,
    approximately_sized = 0b000100000,
    sized               = 0b001100000,
    borrowed            = 0b010000000,
    copyable            = 0b100000000,
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

template <class RangeT, class RefT, class RValueRefT, class DiffT, any_view_options OptsV>
concept ext_any_compatible_range = /* ... */;

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
    constexpr any_view() noexcept;
    constexpr any_view(const any_view&);
    constexpr any_view(any_view&&) noexcept;

    constexpr any_view& operator=(const any_view&);
    constexpr any_view& operator=(any_view&&) noexcept;

    constexpr ~any_view();

    // [range.any.access]
    constexpr iterator begin();

    constexpr sentinel end();

    constexpr std::make_unsigned_t<DiffT> size() const;

    constexpr std::make_unsigned_t<DiffT> reserve_hint() const;

    // [range.any.swap]
    constexpr void swap(any_view&) noexcept;

    constexpr friend void swap(any_view&, any_view&) noexcept;
};

} // namespace beman::any_view

template <class ElementT, beman::any_view::any_view_options OptsV, class RefT, class RValueRefT, class DiffT>
inline constexpr bool std::ranges::enable_borrowed_range<
    beman::any_view::any_view<ElementT, OptsV, RefT, RValueRefT, DiffT>> =
        (OptsV & beman::any_view::any_view_options::borrowed) == beman::any_view::any_view_options::borrowed;
```

</details>

### Configuration Options

The `any_view_options` enum controls the capabilities exposed by the view. Combine options using the `|` operator:

| Option | Purpose |
|--------|---------|
| `input` | Iterator only models `std::input_iterator` (Required) |
| `forward` | Iterator models `std::forward_iterator` |
| `bidirectional` | Iterator models `std::bidirectional_iterator` |
| `random_access` | Iterator models `std::random_access_iterator` |
| `contiguous` | Iterator models `std::contiguous_iterator` |
| `approximately_sized` | Provides `reserve_hint()` for approximate size |
| `sized` | Provides `size()` |
| `borrowed` | Enables `std::ranges::borrowed_range` for iterator lifetime extension |
| `copyable` | View is copyable; otherwise move-only |

### Template Parameters

- `ElementT`: The element type exposed by the view
- `OptsV`: Capability flags (default: `input`)
- `RefT`: The reference type returned by `operator*` (default: `ElementT&`)
- `RValueRefT`: The rvalue reference type (default: deduced from `RefT`)
- `DiffT`: Difference type for iteration (default: `std::ptrdiff_t`)

Most code only needs to specify `ElementT` and `OptsV`; other parameters take sensible defaults.

### When to use `any_view`

**Use when:**

- Library functions need to accept ranges but hide implementation coupling
- You want to return views without exposing internal composition
- Supporting many range types without template bloat
- Creating ABI-stable library interfaces with C++ ranges
- Working across compilation boundaries

**Avoid when:**

- The range type is known and fixed at compile time
- Maximum performance is critical and you can't afford runtime polymorphism overhead
- You only handle a single range type

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

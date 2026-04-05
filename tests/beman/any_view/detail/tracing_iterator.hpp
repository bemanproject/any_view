// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <iterator>

template <class IteratorT>
class tracing_iterator {
    IteratorT       iterator;
    std::ptrdiff_t* offset_ptr;

  public:
    using value_type = std::iter_value_t<IteratorT>;

    constexpr tracing_iterator() = default;

    constexpr tracing_iterator(const tracing_iterator& other) = default;

    constexpr explicit tracing_iterator(IteratorT iterator, std::ptrdiff_t* offset_ptr)
        : iterator(iterator), offset_ptr(offset_ptr) {}

    constexpr explicit tracing_iterator(IteratorT iterator) : tracing_iterator(iterator, nullptr) {}

    constexpr std::iter_reference_t<IteratorT> operator*() const { return *iterator; }

    constexpr std::iter_reference_t<IteratorT> operator[](std::ptrdiff_t n) const {
        *offset_ptr = n;
        return iterator[n];
    }

    constexpr tracing_iterator& operator++() {
        ++iterator;
        *offset_ptr = 1;
        return *this;
    }

    constexpr tracing_iterator operator++(int) {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr tracing_iterator& operator--() {
        --iterator;
        *offset_ptr = -1;
        return *this;
    }

    [[nodiscard]] constexpr tracing_iterator operator--(int) {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr tracing_iterator& operator+=(std::ptrdiff_t n) {
        iterator += n;
        *offset_ptr = n;
        return *this;
    }

    constexpr tracing_iterator& operator-=(std::ptrdiff_t n) {
        iterator -= n;
        *offset_ptr = -n;
        return *this;
    }

    [[nodiscard]] constexpr tracing_iterator operator+(std::ptrdiff_t n) const {
        auto tmp = *this;
        tmp += n;
        return tmp;
    }

    [[nodiscard]] constexpr friend tracing_iterator operator+(std::ptrdiff_t n, const tracing_iterator& self) {
        return self + n;
    }

    [[nodiscard]] constexpr tracing_iterator operator-(std::ptrdiff_t n) const {
        auto tmp = *this;
        tmp -= n;
        return tmp;
    }

    [[nodiscard]] constexpr std::ptrdiff_t operator-(const tracing_iterator& other) const {
        return iterator - other.iterator;
    }

    constexpr bool operator==(const tracing_iterator& other) const { return iterator == other.iterator; }

    constexpr auto operator<=>(const tracing_iterator& other) const { return iterator <=> other.iterator; }

    template <std::sentinel_for<IteratorT> SentinelT>
    constexpr bool operator==(const SentinelT& sentinel) const {
        return iterator == sentinel;
    }
};

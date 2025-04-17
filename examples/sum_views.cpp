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

int main() {
    auto res = sum(std::vector{std::vector{1, 2}, std::vector{3, 4}});
    std::cout << res << "\n";
    return 0;
}


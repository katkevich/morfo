#pragma once
#include <algorithm>
#include <vector>

namespace mrf::misc {
template <typename T>
class set {
public:
    std::vector<T> items;

    constexpr T& emplace(const T& key) {
        if (auto it = std::ranges::find(items, key); it != std::ranges::end(items)) {
            return *it;
        } else {
            items.push_back(key);
            return items.back();
        }
    }

    template <typename Fn>
    constexpr void foreach (Fn fn) {
        for (T& it : items) {
            fn(it);
        }
    }
};
} // namespace mrf::misc
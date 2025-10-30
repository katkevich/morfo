#pragma once
#include <algorithm>
#include <vector>

namespace mrf {

// std::unordered_map ain't supported in constant expressions yet hence this custom mrf::unordered_map
template <typename TKey, typename TVal>
class unordered_map {
public:
    struct item {
        TKey key;
        TVal val;
    };

    constexpr TVal& operator[](TKey key) {
        if (auto it = std::ranges::find(items, key, &item::key); it != std::ranges::end(items)) {
            return it->val;
        } else {
            items.push_back(item{ key });
            return items.back().val;
        }
    }

    template <typename Fn>
    constexpr void foreach (Fn fn) {
        for (item& it : items) {
            fn(it.key, it.val);
        }
    }

    std::vector<item> items;
};
} // namespace mrf
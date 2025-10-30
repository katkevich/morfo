#pragma once
#include "morfo/misc/static_vector.hpp"

namespace mrf {
template <typename TKey, typename TValue, std::size_t Capacity>
struct static_map {
    using key_type = TKey;
    using value_type = TValue;

    constexpr TValue& operator[](const TKey& key) {
        if (auto it = std::ranges::find(keys.data, key); it != std::ranges::end(keys.data)) {
            return values.data[it - std::ranges::begin(keys.data)];
        } else {
            keys.push_back(key);
            values.push_back({});

            return values.back();
        }
    }

    mrf::static_vector<TKey, Capacity> keys{};
    mrf::static_vector<TValue, Capacity> values{};
    std::size_t size{};

    static constexpr std::size_t capacity = Capacity;
};

template <auto StaticMap, typename Fn>
constexpr void static_map_foreach(Fn fn) {
    [&fn]<std::size_t... Idx>(std::index_sequence<Idx...>) {
        constexpr std::array keys_array = mrf::static_vector_into_array<StaticMap.keys>();
        constexpr std::array values_array = mrf::static_vector_into_array<StaticMap.values>();
        
        (fn.template operator()<keys_array[Idx], values_array[Idx]>(), ...);
    }(std::make_index_sequence<StaticMap.size>());
}
} // namespace mrf
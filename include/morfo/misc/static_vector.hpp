#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>

namespace mrf {

template <typename T, std::size_t Capacity>
struct static_vector {
    using value_type = T;

    constexpr void push_back(T item) {
        data[size++] = std::move(item);
    }

    constexpr T& back() {
        return data[size - 1];
    }

    std::array<T, Capacity> data{};
    std::size_t size{};

    static constexpr std::size_t capacity = Capacity;
};

template <auto StaticVector>
constexpr auto static_vector_into_array() {
    std::array<typename decltype(StaticVector)::value_type, StaticVector.size> out;
    std::ranges::copy_n(StaticVector.data.begin(), StaticVector.size, out.begin());

    return out;
}

template <auto StaticVector, typename Fn>
constexpr void static_vector_foreach(Fn fn) {
    [&]<std::size_t... Idx>(std::index_sequence<Idx...>) {
        constexpr std::array arr = mrf::static_vector_into_array<StaticVector>();
        (fn.template operator()<arr[Idx]>(), ...);
    }(std::make_index_sequence<StaticVector.size>());
}

template <auto StaticVector, typename Fn>
constexpr decltype(auto) static_vector_spread(Fn fn) {
    return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) -> decltype(auto) {
        constexpr std::array arr = mrf::static_vector_into_array<StaticVector>();
        return fn.template operator()<arr[Idx]...>();
    }(std::make_index_sequence<StaticVector.size>());
}
} // namespace mrf
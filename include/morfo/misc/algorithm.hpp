#pragma once
#include <cstddef>
#include <meta>
#include <algorithm>
#include <utility>
#include <vector>

namespace mrf::misc {
template <char const* String>
static consteval std::size_t strlen() {
    std::size_t length = 0;
    while (String[length] != '\0')
        ++length;
    return length;
}

template <char const* String>
static consteval auto to_array() {
    constexpr std::size_t length = strlen<String>();

    std::array<char, length + 1> arr{}; // + '\0'
    for (std::size_t i = 0; i < length; ++i)
        arr[i] = String[i];

    return arr;
}

template <std::size_t N>
consteval auto make_index_sequence() {
    std::array<std::size_t, N> indices;
    for (std::size_t i = 0; i < N; ++i) {
        indices[i] = i;
    }
    return indices;
}

template <std::meta::info item>
consteval auto name_of() {
    return to_array<define_static_string(identifier_of(item))>();
}

consteval auto nsdm_size_of(std::meta::info type) {
    return nonstatic_data_members_of(type, std::meta::access_context::unchecked()).size();
}

consteval auto nsdm_of(std::meta::info type) {
    return define_static_array(nonstatic_data_members_of(type, std::meta::access_context::unchecked()));
}

template <std::meta::info Type>
consteval auto nsdm_of() {
    return []<std::size_t... Is>(std::index_sequence<Is...>) {
        constexpr auto nsdm = define_static_array(nonstatic_data_members_of(Type, std::meta::access_context::unchecked()));
        return std::array{ nsdm[Is]... };
    }(std::make_index_sequence<nsdm_size_of(Type)>());
}

template <typename TRng, typename TValue>
constexpr auto index_of(TRng&& rng, const TValue& value) {
    return std::ranges::find(rng, value) - std::ranges::begin(rng);
}

template <auto Array, typename Fn>
constexpr void foreach (Fn fn) {
    [&fn]<std::size_t... Idx>(std::index_sequence<Idx...>) { //
        (fn.template operator()<Array[Idx]>(), ...);
    }(std::make_index_sequence<Array.size()>());
}

template <auto Array, typename Fn>
constexpr decltype(auto) spread(Fn fn) {
    return [&fn]<std::size_t... Idx>(std::index_sequence<Idx...>) -> decltype(auto) {
        return fn.template operator()<Array[Idx]...>();
    }(std::make_index_sequence<Array.size()>());
}

template <auto>
struct always_false {
    static constexpr bool value = false;
};

} // namespace mrf::misc
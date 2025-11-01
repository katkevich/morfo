#pragma once
#include <cstddef>
#include <meta>
#include <utility>
#include <vector>

namespace mrf {
namespace str {
template <char const* String>
static consteval std::size_t strlen() {
    std::size_t length = 0;
    while (String[length] != '\0')
        ++length;
    return length;
}

template <char const* String>
static consteval auto to_array() {
    constexpr std::size_t length = mrf::str::strlen<String>();

    std::array<char, length + 1> arr{}; // + '\0'
    for (std::size_t i = 0; i < length; ++i)
        arr[i] = String[i];

    return arr;
}
} // namespace str

template <std::meta::info item>
consteval auto name_of() {
    return mrf::str::to_array<define_static_string(identifier_of(item))>();
}

consteval auto nsdm_of(std::meta::info type) {
    return define_static_array(nonstatic_data_members_of(type, std::meta::access_context::unchecked()));
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

} // namespace mrf
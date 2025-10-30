#pragma once
#include <cstddef>
#include <meta>
#include <utility>
#include <vector>

namespace mrf {

consteval std::vector<std::meta::info> nsdm_of(std::meta::info type) {
    return nonstatic_data_members_of(type, std::meta::access_context::unchecked());
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

} // namespace mrf
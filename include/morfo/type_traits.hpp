#pragma once
#include <type_traits>

namespace mrf {
template <typename T>
struct original {
    using type = typename std::remove_cvref_t<T>::original_type;
};
template <typename T>
using original_t = typename original<T>::type;

template <typename T>
struct storage {
    using type = typename std::remove_cvref_t<T>::storage_type;
};
template <typename T>
using storage_t = typename storage<T>::type;

template <template <typename...> typename TTmpl, typename T>
struct is_specialization_of : std::false_type {};

template <template <typename...> typename TTmpl, typename... TArgs>
struct is_specialization_of<TTmpl, TTmpl<TArgs...>> : std::true_type {};

template <template <typename...> typename TTmpl, typename... TArgs>
struct is_specialization_of<TTmpl, const TTmpl<TArgs...>> : std::true_type {};

template <template <typename...> typename TTmpl, typename... TArgs>
struct is_specialization_of<TTmpl, const TTmpl<TArgs...>&> : std::true_type {};

template <template <typename...> typename TTmpl, typename... TArgs>
struct is_specialization_of<TTmpl, TTmpl<TArgs...>&> : std::true_type {};

template <template <typename...> typename TTmpl, typename... TArgs>
struct is_specialization_of<TTmpl, TTmpl<TArgs...>&&> : std::true_type {};

template <template <typename...> typename TTmpl, typename T>
static constexpr bool is_specialization_of_v = is_specialization_of<TTmpl, T>::value;
} // namespace mrf
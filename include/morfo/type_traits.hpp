#pragma once
#include <type_traits>

namespace mrf {
template <typename T>
struct original_type {
    using type = typename std::remove_cvref_t<T>::original_type;
};
template <typename T>
using original_type_t = typename original_type<T>::type;


template <typename T>
struct vector_type {
    using type = typename std::remove_cvref_t<T>::vector_type;
};
template <typename T>
using vector_type_t = typename vector_type<T>::type;

/**
 * `Storage` here means a class which contains nonstatic data members for `T`.
 * This is not necessarily `T` itself - it could be a base class of `T`
 */
template <typename T, typename = void>
struct storage_type {
    using type = T;
};
template <typename T>
struct storage_type<T, std::void_t<typename std::remove_cvref_t<T>::storage_type>> {
    using type = typename std::remove_cvref_t<T>::storage_type;
};
template <typename T>
using storage_type_t = typename storage_type<T>::type;

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


template <typename TupleLike, std::size_t N>
concept tuple_like_relaxed = []<std::size_t... Is>(std::index_sequence<Is...>) {
    using std::get;
    return requires(TupleLike t) { (get<Is>(t), ...); };
}(std::make_index_sequence<N>{});
} // namespace mrf
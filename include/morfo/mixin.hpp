#pragma once
#include "morfo/type_traits.hpp"
#include <compare>
#include <vector>

namespace mrf::mixin {

struct into_mixin {
    /* Copy the content of 'morfo' into 'TInto' type (original type by default) */
    template <typename TInto, typename TSelf>
    constexpr auto into(this const TSelf& self) {
        return self.template forward_into<TInto>();
    }

    /* "Steal" (move from lvalue/rvalue) the content of 'morfo' reference into 'TInto' type (original type by default) */
    template <typename TInto, typename TSelf>
    constexpr auto steal_into(this TSelf&& self) {
        return std::move(self).template forward_into<TInto>();
    }

    /* Copy (from lvalue) or "steal" (move from rvalue) the content of 'morfo' reference into 'TInto' type (original type by default) */
    template <typename TInto, typename TSelf>
    constexpr auto forward_into(this TSelf&& self) {
        return misc::spread<misc::nsdm_of<^^mrf::storage_type_t<TSelf>>()>([&self]<std::meta::info... Members> { //
            return TInto{ std::forward_like<TSelf>(self.[:Members:])... };
        });
    }
};

template <typename TValue>
struct into_with_default_mixin {
    /* Copy the content of 'morfo' into 'TInto' type (original type by default) */
    template <typename TInto = TValue, typename TSelf>
    constexpr auto into(this const TSelf& self) {
        return self.template forward_into<TInto>();
    }

    /* "Steal" (move from lvalue/rvalue) the content of 'morfo' reference into 'TInto' type (original type by default) */
    template <typename TInto = TValue, typename TSelf>
    constexpr auto steal_into(this TSelf&& self) {
        return std::move(self).template forward_into<TInto>();
    }

    /* Copy (from lvalue) or "steal" (move from rvalue) the content of 'morfo' reference into 'TInto' type (original type by default) */
    template <typename TInto = TValue, typename TSelf>
    constexpr auto forward_into(this TSelf&& self) {
        return misc::spread<misc::nsdm_of<^^mrf::storage_type_t<TSelf>>()>([&self]<std::meta::info... Members> { //
            return TInto{ std::forward_like<TSelf>(self.[:Members:])... };
        });
    }
};

struct into_tuple_mixin {
    template <typename TSelf>
    constexpr auto into_tuple(this TSelf&& self) {
        return self.forward_into_tuple();
    }

    /* "Steal" (move from lvalue/rvalue) the content of 'morfo' reference into std::tuple */
    template <typename TSelf>
    constexpr auto steal_into_tuple(this TSelf&& self) {
        return std::move(self).forward_into_tuple();
    }

    /* Copy (from lvalue) or "steal" (move from rvalue) the content of 'morfo' reference into std::tuple */
    template <typename TSelf>
    constexpr auto forward_into_tuple(this TSelf&& self) {
        return misc::spread<misc::nsdm_of<^^mrf::storage_type_t<TSelf>>()>([&self]<std::meta::info... Members> {
            return std::make_tuple(std::forward_like<TSelf>(self.[:Members:])...);
        });
    }
};

template <typename TValue>
struct from_mixin {
    template <typename TSelf, typename UOriginal>
        requires std::same_as<TValue, std::remove_cvref_t<UOriginal>>
    constexpr void from(this TSelf&& self, UOriginal&& other) {
        constexpr auto self_nsdm = misc::nsdm_of<^^mrf::storage_type_t<TSelf>>();
        constexpr auto other_nsdm = misc::nsdm_of<^^mrf::storage_type_t<TValue>>();
        constexpr auto Is = misc::make_index_sequence<self_nsdm.size()>();

        template for (constexpr auto I : Is) {
            self.[:self_nsdm[I]:] = std::forward<UOriginal>(other).[:other_nsdm[I]:];
        }
    }

    template <typename TSelf, typename TTuple>
        requires mrf::is_specialization_of_v<std::tuple, TTuple>
    constexpr void from(this TSelf&& self, TTuple&& tuple) {
        constexpr auto nsdm = misc::nsdm_of<^^mrf::storage_type_t<TSelf>>();
        constexpr auto Is = misc::make_index_sequence<nsdm.size()>();

        template for (constexpr auto I : Is) {
            self.[:nsdm[I]:] = std::get<I>(std::forward<TTuple>(tuple));
        }
    }

    template <typename TSelf, typename USelf>
        requires std::same_as<std::remove_cvref_t<TSelf>, std::remove_cvref_t<USelf>>
    constexpr void from(this TSelf&& self, USelf&& other) {
        template for (constexpr auto member : misc::nsdm_of(^^mrf::storage_type_t<TSelf>)) {
            self.[:member:] = other.[:member:];
        }
    }

    template <typename TSelf, typename UOriginal>
        requires std::same_as<TValue, std::remove_cvref_t<UOriginal>>
    constexpr void steal_from(this TSelf&& self, UOriginal&& other) {
        constexpr auto self_nsdm = misc::nsdm_of<^^mrf::storage_type_t<TSelf>>();
        constexpr auto other_nsdm = misc::nsdm_of<^^mrf::storage_type_t<TValue>>();
        constexpr auto Is = misc::make_index_sequence<self_nsdm.size()>();

        template for (constexpr auto I : Is) {
            self.[:self_nsdm[I]:] = std::move(other.[:other_nsdm[I]:]);
        }
    }

    template <typename TSelf, typename TTuple>
        requires mrf::is_specialization_of_v<std::tuple, TTuple>
    constexpr void steal_from(this TSelf&& self, TTuple&& tuple) {
        constexpr auto nsdm = misc::nsdm_of<^^mrf::storage_type_t<TSelf>>();
        constexpr auto Is = misc::make_index_sequence<nsdm.size()>();

        template for (constexpr auto I : Is) {
            self.[:nsdm[I]:] = std::move(std::get<I>(tuple));
        }
    }

    template <typename TSelf, typename USelf>
        requires std::same_as<std::remove_cvref_t<TSelf>, std::remove_cvref_t<USelf>>
    constexpr void steal_from(this TSelf&& self, USelf&& other) {
        template for (constexpr auto member : misc::nsdm_of(^^mrf::storage_type_t<TSelf>)) {
            self.[:member:] = std::move(other.[:member:]);
        }
    }
};

struct collect_mixin {
    template <template <typename...> typename TResultContainer = std::vector, typename TSelf>
    constexpr auto collect(this TSelf&& self) {
        using original_type = typename std::remove_cvref_t<TSelf>::original_type;
        TResultContainer<original_type> result;

        if constexpr (requires { result.reserve(1); }) {
            result.reserve(self.size());
        }

        for (auto&& item : self) {
            if constexpr (requires { result.push_back(std::forward_like<TSelf>(item).forward_into()); }) {
                result.push_back(std::forward_like<TSelf>(item).forward_into());
            } else {
                result.insert(result.end(), std::forward_like<TSelf>(item).forward_into());
            }
        }
        return result;
    }
};

struct cmp_mixin {
    template <typename TSelf>
    constexpr auto operator<=>(this const TSelf& self, const TSelf& other) {
        template for (constexpr auto member : misc::nsdm_of(^^typename TSelf::storage_type)) {
            if (auto res = self.[:member:] <=> other.[:member:]; res != 0) {
                return res;
            }
        }

        return std::strong_ordering::equal;
    }

    template <typename TSelf>
    constexpr auto operator==(this const TSelf& self, const TSelf& other) {
        template for (constexpr auto member : misc::nsdm_of(^^typename TSelf::storage_type)) {
            if (!(self.[:member:] == other.[:member:])) {
                return false;
            }
        }
        return true;
    }
};

template <typename TInto>
struct aggregate_implicit_convert_into_mixin {
    template <typename TSelf>
    constexpr operator TInto(this TSelf&& self) {
        auto& [... members] = self;
        return TInto{ std::forward_like<TSelf>(members)... };
    }
};
} // namespace mrf::mixin
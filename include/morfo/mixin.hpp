#pragma once
#include <compare>
#include <vector>

namespace mrf::mixin {

struct into_mixin {
    /* Copy the content of 'morfo' reference back into its original type */
    template <typename TSelf>
    constexpr auto into(this const TSelf& self) {
        return self.forward_into();
    }

    /* Copy or "steal" (move form rvalue) the content of 'morfo' reference back into its original type */
    template <typename TSelf>
    constexpr auto forward_into(this TSelf&& self) {
        using reference_type = std::remove_cvref_t<TSelf>;
        using original_type = typename reference_type::original_type;

        return [&self]<std::size_t... Is>(std::index_sequence<Is...>) {
            constexpr auto nsdm = mrf::nsdm_of(^^reference_type);
            return original_type{ std::forward_like<TSelf>(self.[:nsdm[Is]:])... };
        }(std::make_index_sequence<mrf::nsdm_size_of(^^reference_type)>{});
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
        template for (constexpr auto member : mrf::nsdm_of(^^typename TSelf::storage_type)) {
            if (auto res = self.[:member:] <=> other.[:member:]; res != 0) {
                return res;
            }
        }
        
        return std::strong_ordering::equal;
    }

    template <typename TSelf>
    constexpr auto operator==(this const TSelf& self, const TSelf& other) {
        template for (constexpr auto member : mrf::nsdm_of(^^typename TSelf::storage_type)) {
            if (!(self.[:member:] == other.[:member:])) {
                return false;
            }
        }
        return true;
    }
};
} // namespace mrf::mixin
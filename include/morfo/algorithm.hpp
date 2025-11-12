#pragma once
#include "morfo/vector.hpp"

namespace mrf {
/* Customization Point Object aka "niebloid" */
namespace cpo {

struct into {
    /* `mrf::vector<T>::reference/mrf::vector<T>::const_reference` */
    template <typename TMrfReference>
    static constexpr auto operator()(const TMrfReference& ref) {
        return ref.into();
    }
};

struct collect {
    /* `mrf::vector<T>` */
    template <template <typename...> typename TTargetContainer = std::vector, typename TMrfContainer>
    static constexpr auto operator()(TMrfContainer&& container) {
        return std::forward<TMrfContainer>(container).template collect<TTargetContainer>();
    }
};
} // namespace cpo

inline constexpr cpo::into into{};
inline constexpr cpo::collect collect{};
} // namespace mrf
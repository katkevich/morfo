#pragma once
#include "morfo/misc/algorithm.hpp"
#include <algorithm>

namespace mrf::proj {
template <auto MemberMetaInfo>
struct member_t {
    template <typename T>
    constexpr auto& operator()(mrf::vector<T>& morfo_container, std::size_t idx) {
        constexpr auto stats = mrf::vector<T>::collect_member_stats();
        constexpr auto stat = *std::ranges::find(stats, MemberMetaInfo, &mrf::vector<T>::member_stat::item_member);

        return morfo_container.storage.[:stat.storage_member:][idx].[:stat.bucket_member:];
    }

    template <typename TRef>
    constexpr auto& operator()(TRef& ref) {
        constexpr auto ref_nsdm = mrf::nsdm_of(^^typename TRef::storage_type);
        constexpr auto orig_nsdm = mrf::nsdm_of(^^typename TRef::original_type);

        constexpr auto index_of = std::ranges::find(orig_nsdm, MemberMetaInfo) - std::ranges::begin(orig_nsdm);

        return ref.[:ref_nsdm[index_of]:];
    }
};

template <auto MemberMetaInfo>
static constexpr member_t<MemberMetaInfo> member{};
} // namespace mrf::proj
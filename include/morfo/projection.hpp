#pragma once
#include "morfo/bucket.hpp"
#include "morfo/misc/algorithm.hpp"
#include "morfo/vector.hpp"
#include <algorithm>

namespace mrf::proj {
template <auto MetaInfo>
    requires cpt::member_meta<MetaInfo>
struct member_t {
    template <typename T>
    constexpr auto& operator()(mrf::vector<T>& morfo_container, std::size_t idx) {
        constexpr auto stats = mrf::vector<T>::collect_member_stats();
        constexpr auto stat = *std::ranges::find(stats, MetaInfo, &mrf::vector<T>::member_stat::item_member);

        return morfo_container.storage.[:stat.storage_member:][idx].[:stat.bucket_member:];
    }

    template <typename TRef>
    constexpr auto& operator()(TRef& ref) {
        constexpr auto ref_nsdm = misc::nsdm_of(^^typename TRef::storage_type);
        constexpr auto orig_nsdm = misc::nsdm_of(^^typename TRef::original_type);

        constexpr auto index_of = std::ranges::find(orig_nsdm, MetaInfo) - std::ranges::begin(orig_nsdm);

        return ref.[:ref_nsdm[index_of]:];
    }
};

template <auto Id>
    requires cpt::bucket_id<Id>
struct bucket_t {
    template <typename T>
    constexpr auto operator()(mrf::vector<T>& morfo_container, std::size_t idx) {
        auto& bucket = morfo_container.template bucket<Id>()[idx];
        auto& [... members] = bucket;

        return mrf::bucket_reference<T, Id>{ members... };
    }

    template <typename TRef>
    constexpr auto operator()(TRef& ref) {
        using original_type = typename TRef::original_type;
        using bucket_type = mrf::bucket<original_type, Id>;
        using bucket_reference = mrf::bucket_reference<original_type, Id>;

        constexpr auto bucket_nsdm = misc::nsdm_of<^^typename bucket_type::storage_type>();

        return misc::spread<bucket_nsdm>([&]<auto... BucketMembers>() {
            constexpr auto ref_nsdm = misc::nsdm_of<^^typename TRef::storage_type>();
            return bucket_reference{ ref.[:*std::ranges::find(ref_nsdm, identifier_of(BucketMembers), &std::meta::identifier_of):]... };
        });
    }
};

template <auto MetaInfo>
    requires cpt::member_meta<MetaInfo>
static constexpr member_t<MetaInfo> member{};

template <auto Id>
    requires cpt::bucket_id<Id>
static constexpr bucket_t<Id> bucket{};
} // namespace mrf::proj
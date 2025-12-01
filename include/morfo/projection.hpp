#pragma once
#include "morfo/bucket.hpp"
#include "morfo/misc/algorithm.hpp"
#include "morfo/vector.hpp"
#include <algorithm>

namespace mrf {
namespace proj {
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

struct into_t {
    /* `mrf::vector<T>::reference/mrf::vector<T>::const_reference` */
    template <typename TMrfReference>
    static constexpr auto operator()(const TMrfReference& ref) {
        return ref.into();
    }
};

struct from_t {
    template <typename T>
    static constexpr auto operator()(const T& value) {
        auto& [... members] = value;
        return mrf::const_reference<T>{ members... };
    }

    template <typename T>
    static constexpr auto operator()(T& value) {
        auto& [... members] = value;
        return mrf::reference<T>{ members... };
    }
};

struct collect_t {
    /* `mrf::vector<T>` */
    template <template <typename...> typename TTargetContainer = std::vector, typename TMrfContainer>
    static constexpr auto operator()(TMrfContainer&& container) {
        return std::forward<TMrfContainer>(container).template collect<TTargetContainer>();
    }
};

/**
 * Functor which converts Morfo references back into its original type.
 * Usage example:
 *
 * mrf::vector<Person> morfo;
 * std::vector<Person> original;
 * std::ranges::transform(morfo, std::back_inserter(original), mrf::into);
 */
inline constexpr into_t into{};

/**
 * Functor which converts original type into Morfo reference.
 * Usage example:
 *
 * mrf::vector<Person> morfo;
 * std::vector<Person> original;
 * std::ranges::transform(original, std::back_inserter(morfo), mrf::from);
 */
inline constexpr from_t from{};

/**
 * Functor which convert Morfo collection into desired collection with 'original type' elements.
 * Usage example:
 *
 * mrf::vector<Person> morfo;
 * std::vector<Person> original = mrf::collect(morfo); // same as morfo.collect();
 */
inline constexpr collect_t collect{};

/**
 * Functor (projection) which given a reflection of an original member returns a reference to a member of a
 * mrf::referece<...>.
 * Usage example:
 *
 * mrf::vector<Person> mrf_persons;
 * mrf::sort(mrf_persons, std::less{}, mrf::proj::member<^^Person::id>); // sort mrf_persons by 'id'
 */
template <auto MetaInfo>
    requires cpt::member_meta<MetaInfo>
inline constexpr member_t<MetaInfo> member{};

/**
 * Functor (projection) which given a bucket ID (tag or member reflection) returns a reference to a corresponding
 * bucket (corresponding bucket should exist).
 * Usage example:
 *
 * mrf::vector<Person> mrf_persons;
 * mrf::sort(mrf_persons, std::less{}, mrf::proj::bucket<mrf::hot>); // sort by 'mrf::bucket_reference<T, mrf::hot>' bucket
 * mrf::sort(mrf_persons, std::less{}, mrf::proj::bucket<^^Person::id>); // sort by mrf::bucket_reference<T, ^^Person::id> bucket
 */
template <auto Id>
    requires cpt::bucket_id<Id>
inline constexpr bucket_t<Id> bucket{};
} // namespace proj

inline constexpr proj::into_t into{};
inline constexpr proj::from_t from{};
inline constexpr proj::collect_t collect{};
} // namespace mrf
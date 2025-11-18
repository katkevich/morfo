#pragma once
#include "morfo/type_traits.hpp"
#include <meta>

namespace mrf {
template <typename Tag>
struct bucket_tag {};

/**
 * Annotation which will group several members into a single bucket.
 *
 * struct [[= mrf::cold]] Person {
 *      [[= mrf::hot]] int id{};            // goes into `mrf::bucket<Person, mrf::hot>` bucket
 *      [[= mrf::hot]] std::string name{};  // goes into `mrf::bucket<Person, mrf::hot>` bucket
 *      std::string address{};              // goes into `mrf::bucket<Person, mrf::cold>` bucket
 * }
 */
template <typename Tag>
inline constexpr bucket_tag<Tag> tag;

/**
 * Predefined mrf::bucket_tag<...> annotations
 */
struct hot_tag;
struct cold_tag;
struct archive_tag;

inline constexpr auto hot = tag<hot_tag>;
inline constexpr auto cold = tag<cold_tag>;
inline constexpr auto archive = tag<archive_tag>;

namespace cpt {
/**
 * Annotated bucket. `mrf::bucket<Person, mrf::hot>`
 *                                        ^^^^^^^^ this one
 */
template <auto Tag>
concept bucket_tag = mrf::is_specialization_of_v<mrf::bucket_tag, decltype(Tag)>;

/**
 * Non-annotated bucket. `mrf::bucket<Person, ^^Person::name>`
 *                                            ^^^^^^^^^^^^^^ this one
 */
template <auto MemberMeta>
concept member_meta = std::meta::is_nonstatic_data_member(MemberMeta);

/* Id is an NTTP which identifies a bucket. */
template <auto Id>
concept bucket_id = bucket_tag<Id> || member_meta<Id>;
} // namespace cpt
} // namespace mrf
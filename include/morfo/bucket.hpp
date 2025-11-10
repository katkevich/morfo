#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <ranges>

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



template <std::size_t N>
struct bucket_name {
    std::array<char, N> name = {};

    constexpr bucket_name(const char (&name)[N]) {
        std::ranges::copy_n(name, N, this->name.begin());
    }

    constexpr bucket_name(std::array<char, N> name) {
        std::ranges::copy_n(name.begin(), N, this->name.begin());
    }
};

template <std::size_t N>
bucket_name(const char (&)[N]) -> bucket_name<N>;

template <std::size_t N>
bucket_name(std::array<char, N>) -> bucket_name<N>;


/**
 * `mrf::bucket_id<...>` encapsulates either the tag (annotation) of the name (member name) which identifies the `mrf::bucket<...>`.
 * 
 * ```
 * struct Person {
 *      [[= mrf::hot]] int id{};
 *      int age{};
 *      [[= mrf::hot]] int name{};
 *      [[= mrf::cold]]std::string surname{};
 * };
 * ```
 * 
 * `mrf::bucket<Person, mrf::hot>` is a bucket which stores `Person::id` and `Person::name` members
 * `mrf::bucket<Person, "age">` is a bucket which stores `Person::age` member
 * `mrf::bucket<Person, mrf::cold>` is a bucket which stores `Person::surname` member
 */
struct named_bucket_tag;

template <std::size_t N, typename Tag>
struct bucket_id {
    using tag_t = Tag;
    std::array<char, N> name = {};

    constexpr bucket_id(const char (&name)[N]) {
        std::ranges::copy_n(name, N, this->name.begin());
    }

    constexpr bucket_id(std::array<char, N> name) {
        std::ranges::copy_n(name.begin(), N, this->name.begin());
    }

    constexpr bucket_id(mrf::bucket_tag<Tag>) {}

    constexpr bool is_named() const {
        return std::is_same_v<Tag, mrf::named_bucket_tag>;
    }
};

template <std::size_t N>
bucket_id(const char (&)[N]) -> bucket_id<N, named_bucket_tag>;

template <std::size_t N>
bucket_id(std::array<char, N>) -> bucket_id<N, named_bucket_tag>;

template <typename Tag>
bucket_id(mrf::bucket_tag<Tag>) -> bucket_id<0, Tag>;

} // namespace mrf
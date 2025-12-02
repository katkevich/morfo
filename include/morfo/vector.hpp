#pragma once
#include "morfo/bucket.hpp"
#include "morfo/misc/algorithm.hpp"
#include "morfo/misc/static_vector.hpp"
#include "morfo/misc/unordered_map.hpp"
#include "morfo/mixin.hpp"
#include <cassert>

namespace mrf {
namespace proj {
template <auto MetaInfo>
    requires cpt::member_meta<MetaInfo>
struct member_t;

template <auto Id>
    requires cpt::bucket_id<Id>
struct bucket_t;
} // namespace proj

template <typename T>
class vector : public mrf::mixin::collect_mixin {
    static constexpr std::size_t members_count = nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()).size();
    static_assert(members_count > 0, "type T should have at least one nonstatic data member");
    static_assert(!std::is_const_v<T>, "const T ain't supported");
    static_assert(!std::is_reference_v<T>, "reference T ain't supported");

    template <auto MetaInfo>
        requires cpt::member_meta<MetaInfo>
    friend struct proj::member_t;

    template <auto Id>
        requires cpt::bucket_id<Id>
    friend struct proj::bucket_t;

    /* These will be defined using reflection */
private:
    template <auto Id>
    struct bucket_storage_type;
    template <auto Id>
    struct bucket_reference_storage_type;
    template <auto Id>
    struct bucket_const_reference_storage_type;
    struct reference_storage_type;
    struct const_reference_storage_type;
    struct storage_type;
    /**/

    struct member_stat {
        std::meta::info item_member;
        std::meta::info bucket_member;
        std::meta::info storage_member;
    };

    struct bucket_member_stat {
        std::meta::info item_member;
        std::meta::info bucket_member;
    };

    struct storage_member_stat {
        std::meta::info storage_member;
        misc::static_vector<bucket_member_stat, members_count> bucket_members;
    };

    template <std::meta::info Member>
    static consteval std::meta::info get_bucket_id() {
        template for (constexpr auto annotation : define_static_array(annotations_of(Member))) {
            if constexpr (template_of(type_of(annotation)) == ^^mrf::bucket_tag) {
                return std::meta::reflect_constant(extract<typename[:type_of(annotation):]>(annotation));
            }
        }

        template for (constexpr auto annotation : define_static_array(annotations_of(^^T))) {
            if constexpr (template_of(type_of(annotation)) == ^^mrf::bucket_tag) {
                return std::meta::reflect_constant(extract<typename[:type_of(annotation):]>(annotation));
            }
        }

        return std::meta::reflect_constant(Member);
    }

    static consteval void define_bucket_storage_types() {
        misc::unordered_map<std::meta::info, std::vector<std::meta::info>> member_specs;

        template for (constexpr auto member : misc::nsdm_of(^^T)) {
            // clang-format off
            const auto type_info = substitute(^^bucket_storage_type, { get_bucket_id<member>() });
            const auto member_spec = data_member_spec(type_of(member), { .name = identifier_of(member) });
            // clang-format on

            member_specs[type_info].push_back(member_spec);
        }

        member_specs.foreach (
            [](const auto type_info, const auto member_specs) { define_aggregate(type_info, member_specs); });
    }

    static consteval void define_bucket_reference_types(std::meta::info ref_type, bool is_const) {
        misc::unordered_map<std::meta::info, std::vector<std::meta::info>> member_specs;

        template for (constexpr auto member : misc::nsdm_of(^^T)) {
            // clang-format off
            const auto ref_type_info = substitute(ref_type, { get_bucket_id<member>() });
            const auto member_type = type_of(member);
            const auto ref_member_type = add_lvalue_reference(is_const ? add_const(member_type) : member_type);
            const auto ref_member_spec = data_member_spec(ref_member_type, { .name = identifier_of(member) });
            // clang-format on

            member_specs[ref_type_info].push_back(ref_member_spec);
        }

        member_specs.foreach (
            [](const auto type_info, const auto member_specs) { define_aggregate(type_info, member_specs); });
    }

    static consteval void define_storage_type() {
        std::vector<std::meta::info> storage_member_specs;

        template for (constexpr auto member : misc::nsdm_of(^^T)) {
            // clang-format off
            constexpr auto bucket_type_info = substitute(^^bucket_type, { get_bucket_id<member>() });

            constexpr auto storage_member_type = substitute(^^std::vector, { bucket_type_info });
            constexpr auto storage_member_spec = data_member_spec(storage_member_type);
            // clang-format on

            storage_member_specs.push_back(storage_member_spec);
        }

        define_aggregate(^^storage_type, storage_member_specs);
    }

    static consteval void define_reference_type(std::meta::info ref_type, bool is_const) {
        const auto member_stats = collect_member_stats();

        std::vector<std::meta::info> ref_member_specs;

        for (const auto stat : member_stats) {
            const auto member_type = type_of(stat.item_member);
            const auto ref_member_type = add_lvalue_reference(is_const ? add_const(member_type) : member_type);
            const auto ref_member_name = identifier_of(stat.item_member);

            ref_member_specs.push_back(data_member_spec(ref_member_type, { .name = ref_member_name }));
        }

        define_aggregate(ref_type, ref_member_specs);
    }

    static consteval auto collect_member_stats() {
        const auto storage_members = misc::nsdm_of(^^storage_type);
        constexpr auto members = misc::nsdm_of(^^T);

        std::array<member_stat, members_count> stats;

        template for (std::size_t idx = 0; constexpr auto member : members) {
            // clang-format off
            const auto bucket_type_info = substitute(^^bucket_type, { get_bucket_id<member>() });
            const auto storage_member_type = substitute(^^std::vector, { bucket_type_info });
            
            const auto bucket_storage_type_info = substitute(^^bucket_storage_type, { get_bucket_id<member>() });
            const auto bucket_members = misc::nsdm_of(bucket_storage_type_info);
            // clang-format on

            const auto bucket_member = *std::ranges::find(bucket_members, identifier_of(member), &std::meta::identifier_of);
            const auto storage_member = *std::ranges::find(storage_members, storage_member_type, &std::meta::type_of);

            stats[idx++] = member_stat{
                .item_member = member,
                .bucket_member = bucket_member,
                .storage_member = storage_member,
            };
        }

        return stats;
    }

    static consteval auto collect_storage_stats() {
        misc::static_vector<storage_member_stat, members_count> storage_stats{};

        constexpr auto storage_members = misc::nsdm_of(^^storage_type);
        constexpr auto members = misc::nsdm_of(^^T);

        template for (constexpr auto member : members) {
            // clang-format off
            constexpr auto bucket_type_info = substitute(^^bucket_type, { get_bucket_id<member>() });
            constexpr auto storage_member_type = substitute(^^std::vector, { bucket_type_info });
            
            constexpr auto bucket_storage_type_info = substitute(^^bucket_storage_type, { get_bucket_id<member>() });
            constexpr auto bucket_members = misc::nsdm_of(bucket_storage_type_info);
            // clang-format on

            constexpr auto bucket_member = *std::ranges::find(bucket_members, identifier_of(member), &std::meta::identifier_of);
            constexpr auto storage_member = *std::ranges::find(storage_members, storage_member_type, &std::meta::type_of);

            auto storage_stat_it = std::ranges::find(storage_stats.data, storage_member, &storage_member_stat::storage_member);

            if (storage_stat_it == std::ranges::end(storage_stats.data)) {
                storage_stats.push_back(storage_member_stat{
                    .storage_member = storage_member,
                });
                storage_stats.back().bucket_members.push_back(bucket_member_stat{ member, bucket_member });
            } else {
                storage_stat_it->bucket_members.push_back(bucket_member_stat{ member, bucket_member });
            }
        }

        return storage_stats;
    }

public:
    consteval {
        define_bucket_storage_types();
        define_bucket_reference_types(^^bucket_reference_storage_type, false);
        define_bucket_reference_types(^^bucket_const_reference_storage_type, true);
    }

    template <auto Id>
    struct bucket_type : bucket_storage_type<Id>,
                         mrf::mixin::make_mixin<bucket_type<Id>>,
                         mrf::mixin::into_tuple_mixin,
                         mrf::mixin::into_mixin,
                         mrf::mixin::cmp_mixin {
        using original_type = T;
        using vector_type = mrf::vector<T>;
        using storage_type = bucket_storage_type<Id>;
    };

    consteval {
        define_storage_type();
        define_reference_type(^^reference_storage_type, false);
        define_reference_type(^^const_reference_storage_type, true);
    }

    static constexpr auto member_stats_s = collect_member_stats();
    static constexpr auto storage_stats_s = collect_storage_stats();

    template <auto Id>
    struct bucket_const_reference : bucket_const_reference_storage_type<Id>,
                                    mrf::mixin::make_mixin<bucket_const_reference<Id>>,
                                    mrf::mixin::into_tuple_mixin,
                                    mrf::mixin::into_with_default_mixin<bucket_type<Id>>,
                                    mrf::mixin::cmp_mixin {
        using original_type = T;
        using value_type = bucket_type<Id>;
        using vector_type = mrf::vector<T>;
        using storage_type = bucket_const_reference_storage_type<Id>;
    };

    template <auto Id>
    struct bucket_reference : bucket_reference_storage_type<Id>,
                              mrf::mixin::make_mixin<bucket_reference<Id>>,
                              mrf::mixin::into_tuple_mixin,
                              mrf::mixin::into_with_default_mixin<bucket_type<Id>>,
                              mrf::mixin::from_mixin<bucket_type<Id>>,
                              mrf::mixin::cmp_mixin,
                              mrf::mixin::aggregate_implicit_convert_into_mixin<bucket_const_reference<Id>> {
        using original_type = T;
        using value_type = bucket_type<Id>;
        using vector_type = mrf::vector<T>;
        using storage_type = bucket_reference_storage_type<Id>;
    };

    struct const_reference : const_reference_storage_type,
                             mrf::mixin::make_mixin<const_reference>,
                             mrf::mixin::into_tuple_mixin,
                             mrf::mixin::into_with_default_mixin<T>,
                             mrf::mixin::cmp_mixin {
        using original_type = T;
        using value_type = T;
        using vector_type = mrf::vector<T>;
        using storage_type = const_reference_storage_type;
    };

    struct reference : reference_storage_type,
                       mrf::mixin::make_mixin<reference>,
                       mrf::mixin::into_tuple_mixin,
                       mrf::mixin::into_with_default_mixin<T>,
                       mrf::mixin::from_mixin<T>,
                       mrf::mixin::cmp_mixin,
                       mrf::mixin::aggregate_implicit_convert_into_mixin<const_reference> {
        using original_type = T;
        using value_type = T;
        using vector_type = mrf::vector<T>;
        using storage_type = reference_storage_type;
    };

    struct pointer : reference_storage_type {
        using original_type = T;
        using value_type = T;
        using vector_type = mrf::vector<T>;
        using storage_type = reference_storage_type;

        constexpr auto operator->() {
            return this;
        }
        constexpr auto operator->() const {
            return this;
        }
    };

    struct const_pointer : const_reference_storage_type {
        using original_type = T;
        using value_type = T;
        using vector_type = mrf::vector<T>;
        using storage_type = const_reference_storage_type;

        constexpr auto operator->() {
            return this;
        }
        constexpr auto operator->() const {
            return this;
        }
    };

private:
    enum class iter_kind { constant, regular };

    template <iter_kind Kind>
    class ref_iterator {
        template <iter_kind OtherKind>
        friend class ref_iterator;

    public:
        using container_type = std::conditional_t<Kind == iter_kind::constant, const mrf::vector<T>, mrf::vector<T>>;
        using reference = std::conditional_t<Kind == iter_kind::constant, vector::const_reference, vector::reference>;
        using value_type = reference;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<Kind == iter_kind::constant, vector::const_pointer, vector::pointer>;

        constexpr ref_iterator() = default;
        constexpr ref_iterator(container_type* container, std::size_t idx)
            : container(container)
            , idx(idx) {}
        constexpr ref_iterator(const ref_iterator&) = default;
        constexpr ref_iterator(ref_iterator&&) = default;
        constexpr ref_iterator& operator=(const ref_iterator&) = default;
        constexpr ref_iterator& operator=(ref_iterator&&) = default;

        /* non-constant to constant iterator implicit convertion */
        constexpr ref_iterator(const ref_iterator<iter_kind::regular>& that)
            requires(Kind == iter_kind::constant)
            : container(that.container)
            , idx(that.idx) {}

        constexpr reference operator*() const noexcept {
            return misc::spread<member_stats_s>([this]<member_stat... Stats> {
                return reference{ container->storage.[:Stats.storage_member:][idx].[:Stats.bucket_member:]... };
            });
        }

        constexpr pointer operator->() const noexcept {
            return misc::spread<member_stats_s>([this]<member_stat... Stats> {
                return pointer{ container->storage.[:Stats.storage_member:][idx].[:Stats.bucket_member:]... };
            });
        }

        constexpr ref_iterator& operator++() noexcept {
            ++idx;
            return *this;
        }

        constexpr ref_iterator operator++(int) noexcept {
            const auto copy = *this;
            ++idx;
            return copy;
        }

        constexpr ref_iterator& operator--() noexcept {
            --idx;
            return *this;
        }

        constexpr ref_iterator operator--(int) noexcept {
            const auto copy = *this;
            --idx;
            return copy;
        }

        constexpr ref_iterator& operator-=(difference_type offset) noexcept {
            idx -= offset;
            return *this;
        }

        constexpr ref_iterator& operator+=(difference_type offset) noexcept {
            idx += offset;
            return *this;
        }

        constexpr reference operator[](difference_type offset) const {
            auto copy = *this;
            copy += offset;
            return *copy;
        }

    private:
        constexpr friend auto operator<=>(const ref_iterator& l, const ref_iterator& r) noexcept {
            if (!std::is_constant_evaluated()) {
                assert(l.container == r.container);
            }

            /* We cannot compare nullptr and non-nullptr pointers using operator<=> at compile time.
             * Comparison between pointers to unrelated objects (nullptr and non-nullptr) has unspecified value.
             * Which means we have to define the ordering manually here.
             */
            if (!l.container && !r.container) {
                return l.idx <=> r.idx;
            } else if (!l.container) {
                return std::strong_ordering::less;
            } else if (!r.container) {
                return std::strong_ordering::greater;
            }

            if (auto ord = l.container <=> r.container; ord != 0) {
                return ord;
            }

            const auto l_size = l.container->size();
            const auto l_idx_norm = std::min(l.idx, l_size);
            const auto r_idx_norm = std::min(r.idx, l_size);

            return l_idx_norm <=> r_idx_norm;
        }

        constexpr friend bool operator<(const ref_iterator& l, const ref_iterator& r) noexcept {
            return (l <=> r) < 0;
        }

        constexpr friend bool operator<=(const ref_iterator& l, const ref_iterator& r) noexcept {
            return (l <=> r) <= 0;
        }

        constexpr friend bool operator>(const ref_iterator& l, const ref_iterator& r) noexcept {
            return (l <=> r) > 0;
        }

        constexpr friend bool operator>=(const ref_iterator& l, const ref_iterator& r) noexcept {
            return (l <=> r) >= 0;
        }

        constexpr friend bool operator==(const ref_iterator& l, const ref_iterator& r) noexcept {
            return (l <=> r) == 0;
        }

        constexpr friend bool operator!=(const ref_iterator& l, const ref_iterator& r) noexcept {
            return (l <=> r) != 0;
        }

        constexpr friend ref_iterator operator+(ref_iterator that, difference_type offset) noexcept {
            that.idx += offset;
            return that;
        }

        constexpr friend ref_iterator operator+(difference_type offset, ref_iterator that) noexcept {
            that.idx += offset;
            return that;
        }

        constexpr friend ref_iterator operator-(ref_iterator that, difference_type offset) noexcept {
            that.idx -= offset;
            return that;
        }

        constexpr friend difference_type operator-(const ref_iterator& l, const ref_iterator& r) noexcept {
            return difference_type(l.idx) - difference_type(r.idx);
        }

    private:
        container_type* container = {};
        std::size_t idx = std::numeric_limits<std::size_t>::max();
    };

public:
    using original_type = T;
    using value_type = reference;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = ref_iterator<iter_kind::regular>;
    using const_iterator = ref_iterator<iter_kind::constant>;

    template <auto Id>
        requires cpt::bucket_id<Id>
    constexpr const auto& bucket() const {
        return bucket_impl<Id>();
    }

    /**
     * Be careful with changing the size of a mutable bucket!
     * Using mrf::vector while buckets have different size is UB!
     */
    template <auto Id>
        requires cpt::bucket_id<Id>
    constexpr auto& bucket() {
        return bucket_impl<Id>();
    }

    constexpr auto begin() {
        return iterator{ this, 0 };
    }

    constexpr auto end() {
        return iterator{ this, size() };
    }

    constexpr auto begin() const {
        return const_iterator{ this, 0 };
    }

    constexpr auto end() const {
        return const_iterator{ this, size() };
    }

    constexpr auto cbegin() {
        return const_iterator{ this, 0 };
    }

    constexpr auto cend() {
        return const_iterator{ this, size() };
    }

    constexpr auto cbegin() const {
        return const_iterator{ this, 0 };
    }

    constexpr auto cend() const {
        return const_iterator{ this, size() };
    }

    constexpr void push_back(const T& item) {
        push_back_impl(item);
    }

    constexpr void push_back(T&& item) {
        push_back_impl(std::move(item));
    }

    constexpr void push_back(const reference& ref) {
        push_back_ref_impl(ref);
    }

    constexpr void push_back(const const_reference& ref) {
        push_back_ref_impl(ref);
    }

    template <typename... Args>
    constexpr reference emplace_back(Args&&... args) {
        push_back_impl(T{ std::forward<Args>(args)... });
        return back();
    }

    constexpr reference back() {
        return *(--end());
    }

    constexpr const_reference back() const {
        return *(--end());
    }

    constexpr reference front() {
        return *begin();
    }

    constexpr const_reference front() const {
        return *begin();
    }

    constexpr reference operator[](size_type idx) {
        return *(begin() + idx);
    }

    constexpr const_reference operator[](size_type idx) const {
        return *(begin() + idx);
    }

    constexpr reference at(size_type idx) {
        if (idx >= size()) {
            throw std::out_of_range("out of range");
        }
        return *(begin() + idx);
    }

    constexpr const_reference at(size_type idx) const {
        if (idx >= size()) {
            throw std::out_of_range("out of range");
        }
        return *(begin() + idx);
    }

    [[nodiscard]] constexpr bool empty() const {
        return storage.[:misc::nsdm_of(^^storage_type)[0]:].empty();
    }

    constexpr size_type size() const {
        return storage.[:misc::nsdm_of(^^storage_type)[0]:].size();
    }

    constexpr size_type capacity() const {
        return storage.[:misc::nsdm_of(^^storage_type)[0]:].capacity();
    }

    constexpr void reserve(size_type new_cap) {
        misc::static_vector_foreach<storage_stats_s>([new_cap, this]<storage_member_stat StorageMemberStat> {
            storage.[:StorageMemberStat.storage_member:].reserve(new_cap);
        });
    }

    constexpr void shrink_to_fit() {
        misc::static_vector_foreach<storage_stats_s>([this]<storage_member_stat StorageMemberStat> {
            storage.[:StorageMemberStat.storage_member:].shrink_to_fit();
        });
    }

    constexpr void clear() {
        misc::static_vector_foreach<storage_stats_s>([this]<storage_member_stat StorageMemberStat> { //
            storage.[:StorageMemberStat.storage_member:].clear();
        });
    }

    constexpr void pop_back() {
        misc::static_vector_foreach<storage_stats_s>([this]<storage_member_stat StorageMemberStat> { //
            storage.[:StorageMemberStat.storage_member:].pop_back();
        });
    }

    constexpr void resize(size_type new_size)
        requires std::is_default_constructible_v<T>
    {
        resize(new_size, T{});
    }

    constexpr void resize(size_type new_size, const T& default_val) {
        misc::static_vector_foreach<storage_stats_s>([&, this]<storage_member_stat StorageMemberStat> {
            misc::static_vector_spread<StorageMemberStat.bucket_members>([&, this]<bucket_member_stat... BucketMemberStats> {
                storage.[:StorageMemberStat.storage_member:].resize(new_size, { default_val.[:BucketMemberStats.item_member:]... });
            });
        });
    }

    constexpr void swap(vector& that) {
        misc::static_vector_foreach<storage_stats_s>([&that, this]<storage_member_stat StorageMemberStat> {
            storage.[:StorageMemberStat.storage_member:].swap(that.storage.[:StorageMemberStat.storage_member:]);
        });
    }

private:
    template <typename U>
    constexpr void push_back_impl(U&& item) {
        misc::static_vector_foreach<storage_stats_s>([&, this]<storage_member_stat StorageMemberStat> {
            misc::static_vector_spread<StorageMemberStat.bucket_members>([&, this]<bucket_member_stat... BucketMemberStats> {
                storage.[:StorageMemberStat.storage_member:].push_back(
                    { { std::forward_like<U>(item.[:BucketMemberStats.item_member:])... } });
            });
        });
    }

    template <typename TRef>
    constexpr void push_back_ref_impl(const TRef& ref) {
        misc::static_vector_foreach<storage_stats_s>([&, this]<storage_member_stat StorageMemberStat> {
            misc::static_vector_spread<StorageMemberStat.bucket_members>([&, this]<bucket_member_stat... BucketMemberStats> {
                constexpr auto ref_nsdm = misc::nsdm_of(^^typename TRef::storage_type);
                constexpr auto orig_nsdm = misc::nsdm_of(^^typename TRef::original_type);

                storage.[:StorageMemberStat.storage_member:].push_back(
                    { { ref.[:ref_nsdm[misc::index_of(orig_nsdm, BucketMemberStats.item_member)]:]... } });
            });
        });
    }

    template <auto Id, typename TSelf>
    constexpr auto& bucket_impl(this TSelf&& self) {
        constexpr auto storage_member_type = ^^std::vector<bucket_type<Id>>;

        constexpr auto storage_members = misc::nsdm_of(^^storage_type);
        constexpr auto found = std::ranges::find(storage_members, storage_member_type, &std::meta::type_of);

        if constexpr (found != std::ranges::end(storage_members)) {
            return self.storage.[:*found:];
        } else {
            if constexpr (cpt::member_meta<Id>) {
                static_assert(misc::always_false<Id>::value,
                    R"(you are trying to get `mrf::bucket<T, ^^T::member>` bucket but `member` is missing from struct `T`)");
            } else {
                static_assert(misc::always_false<Id>::value,
                    R"(you are trying to get `mrf::bucket<T, tag>` bucket but corresponding member (or struct `T`) ain't marked with `tag` annotation)");
            }
            static std::ranges::dangling dummy;
            return dummy;
        }
    }

private:
    storage_type storage;
};

/**
 * ```
 * struct Person {
 *      [[= mrf::hot]] int id{};
 *      int age{};
 *      [[= mrf::hot]] int name{};
 *      [[= mrf::cold]]std::string surname{};
 * };
 * ```
 *
 * `mrf::bucket<Person, mrf::hot>` is a bucket which stores references to `Person::id` and `Person::name` members
 * `mrf::bucket<Person, ^^Person::age>` is a bucket which stores a reference to `Person::age` member
 * `mrf::bucket<Person, mrf::cold>` is a bucket which stores a reference to `Person::surname` member
 */
template <typename T, auto Id>
using bucket = typename mrf::vector<T>::template bucket_type<Id>;

template <typename T, auto Id>
using bucket_reference = typename mrf::vector<T>::template bucket_reference<Id>;

template <typename T, auto Id>
using bucket_const_reference = typename mrf::vector<T>::template bucket_const_reference<Id>;

template <typename T>
using reference = typename mrf::vector<T>::reference;

template <typename T>
using const_reference = typename mrf::vector<T>::const_reference;
} // namespace mrf
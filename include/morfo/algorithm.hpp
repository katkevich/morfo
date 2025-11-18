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

/* Collections smaller than 32 are being sorted using insertion sort */
static constexpr std::ptrdiff_t insertion_sort_threshold = 32;

template <typename Rng>
static constexpr void swap_elements(Rng& rng, std::ptrdiff_t i, std::ptrdiff_t j) {
    std::tuple tmp = rng[i].steal_into_tuple();
    rng[i].steal_from(rng[j]);
    rng[j].steal_from(tmp);
}

template <typename Rng, typename Compare, typename Proj>
static constexpr std::ptrdiff_t
median_of_three_unchecked(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, Compare comp, Proj proj) {
    const std::ptrdiff_t mid = first + ((last - first) >> 1);

    return [=](auto&& a, auto&& b, auto&& c) {
        if (std::invoke(comp, a, b)) {
            if (std::invoke(comp, b, c)) {
                return mid; /* a < b < c */
            } else if (std::invoke(comp, a, c)) {
                return last - 1; /* a < c < b */
            } else {
                return first; /* c < a < b */
            }
        } else if (std::invoke(comp, a, c)) {
            return first; /* b < a < c */
        } else if (std::invoke(comp, b, c)) {
            return last - 1; /* b < c < a */
        } else {
            return mid; /* c < b < a */
        }
    }(proj(rng, first), proj(rng, mid), proj(rng, last - 1));
}

template <typename Rng, typename Compare, typename TProj>
static constexpr std::pair<std::ptrdiff_t, std::ptrdiff_t>
dnf_partition_unchecked(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, std::ptrdiff_t pivot_pos, Compare comp, TProj proj) {

    std::ptrdiff_t lt = first;
    std::ptrdiff_t gt = last - 1;
    std::ptrdiff_t i = first;

    while (i <= gt) {
        if (std::invoke(comp, proj(rng, i), proj(rng, pivot_pos))) {
            /* Keep track pivot_pos if it was involved in swap. */
            if (i == pivot_pos)
                pivot_pos = lt;
            else if (lt == pivot_pos)
                pivot_pos = i;

            swap_elements(rng, i++, lt++);

        } else if (std::invoke(comp, proj(rng, pivot_pos), proj(rng, i))) {
            /* Keep track pivot_pos if it was involved in swap. */
            if (i == pivot_pos)
                pivot_pos = gt;
            else if (gt == pivot_pos)
                pivot_pos = i;

            swap_elements(rng, i, gt--);
        } else {
            ++i;
        }
    }

    return { lt, gt + 1 };
}

template <typename Rng, typename Compare, typename TProj>
static constexpr std::pair<std::ptrdiff_t, std::ptrdiff_t>
partition_pivot_unchecked(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, Compare comp, TProj proj) {
    const std::ptrdiff_t pivot_pos = median_of_three_unchecked(rng, first, last, comp, proj);
    return dnf_partition_unchecked(rng, first, last, pivot_pos, comp, proj);
}


template <typename Rng, typename Compare, typename Proj>
static constexpr void insertsort(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, Compare comp, Proj proj) {
    using reference_type = typename Rng::reference;

    for (std::ptrdiff_t i = first + 1; i < last; ++i) {
        if (std::invoke(comp, proj(rng, i), proj(rng, i - 1))) {
            std::tuple item_as_tuple = rng[i].steal_into_tuple();

            auto& [... item_members] = item_as_tuple;
            reference_type item_reference{ item_members... };
            decltype(auto) item_projected = proj(item_reference);

            std::ptrdiff_t j = i - 1;

            do {
                rng[j + 1].steal_from(rng[j]);
                --j;
            } while (j >= first && std::invoke(comp, item_projected, proj(rng, j)));

            rng[j + 1].steal_from(std::move(item_as_tuple));
        }
    }
}

template <typename Rng, typename Compare, typename Proj>
static constexpr void heapsort(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, Compare comp, Proj proj) {}

template <typename Rng, typename Compare, typename Proj>
static constexpr void
introsort_loop(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, std::ptrdiff_t depth_limit, Compare comp, Proj proj) {
    while (true) {
        if (last - first <= 1) {
            return;
        }
        /* Small collection is effectively sorted with insertion sort. */
        if (last - first <= insertion_sort_threshold) {
            insertsort(rng, first, last, comp, proj);
            return;
        }

        /* Too many quicksort divisions - probably we keep choosing the bad pivot - fallback to heapsort. */
        // if (depth_limit == 0) {
        //     heapsort(rng, first, last, comp, proj);
        //     return;
        // }

        /* [pivot, pivot) */
        const auto [pivot_first, pivot_last] = partition_pivot_unchecked(rng, first, last, comp, proj);

        /* Recurse on smaller partition (trying to make less recursions).
         * Iterate on bigger partition. */
        if (pivot_first - first < last - pivot_last) {
            introsort_loop(rng, first, pivot_first, depth_limit - 1, comp, proj);
            first = pivot_last;
        } else {
            introsort_loop(rng, pivot_last, last, depth_limit - 1, comp, proj);
            last = pivot_first;
        }
    }
}

static constexpr ptrdiff_t lg(ptrdiff_t n) {
    return std::bit_width(std::make_unsigned_t<ptrdiff_t>(n)) - 1;
}

template <typename Rng, typename Compare, typename Proj>
static constexpr void introsort(Rng& rng, std::ptrdiff_t first, std::ptrdiff_t last, Compare comp, Proj proj) {
    if (first != last) {
        const std::ptrdiff_t max_depth = lg(last - first) * 2;
        introsort_loop(rng, first, last, max_depth, comp, proj);
    }
}

template <typename Rng, typename Compare, typename Proj>
static constexpr void introsort(Rng& rng, Compare comp, Proj proj) {
    introsort(rng, 0, rng.size(), comp, proj);
}

template <typename Rng, typename Compare, typename Proj>
static constexpr void insertsort(Rng& rng, Compare comp, Proj proj) {
    insertsort(rng, 0, rng.size(), comp, proj);
}
} // namespace mrf
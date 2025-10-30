#pragma once

namespace mrf {
template <typename Tag>
struct bucket_tag {};

template <typename Tag>
constexpr bucket_tag<Tag> tag;
} // namespace mrf
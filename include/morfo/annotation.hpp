#pragma once

namespace mrf {
struct hot_tag;
struct cold_tag;
struct archive_tag;

template <typename Tag>
struct bucket_tag {};

template <typename Tag>
inline constexpr bucket_tag<Tag> tag;

// predefined tags
inline constexpr auto hot = tag<hot_tag>;
inline constexpr auto cold = tag<cold_tag>;
inline constexpr auto archive = tag<archive_tag>;
} // namespace mrf
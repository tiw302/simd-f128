/* simd_f128_fmt.hpp
 *
 * modern c++ formatting support for 128-bit double-double values.
 * provides std::formatter and {fmt} library specializations.
 *
 * updated 2026-08-09
 * spdx-license-identifier: mit
 * copyright (c) 2026 jirawat siripuk */

#ifndef SIMD_F128_FMT_HPP
#define SIMD_F128_FMT_HPP

#include "simd_f128.hpp"
#include "simd_f128_io.h"

// ███████ ███    ███ ████████
// ██      ████  ████    ██
// █████   ██ ████ ██    ██
// ██      ██  ██  ██    ██
// ██      ██      ██    ██
//
// >>c++20 formatting api
#if defined(__cplusplus) && __cplusplus >= 202002L
#if __has_include(<format>)
#include <format>
#include <string>

template <>
struct std::formatter<f128::float128> : std::formatter<std::string> {
    auto format(const f128::float128& val, std::format_context& ctx) const {
        char buf[128];
        simd_f128_to_string(buf, sizeof(buf), val.data);
        return std::formatter<std::string>::format(std::string(buf), ctx);
    }
};
#endif
#endif

// support for the popular {fmt} library (if included before this header)
#ifdef FMT_FORMAT_H_
template <>
struct fmt::formatter<f128::float128> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const f128::float128& val, FormatContext& ctx) const {
        char buf[128];
        simd_f128_to_string(buf, sizeof(buf), val.data);
        return fmt::formatter<std::string>::format(std::string(buf), ctx);
    }
};
#endif

#endif  // SIMD_F128_FMT_HPP

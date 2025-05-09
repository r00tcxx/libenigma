/*
 * Created on Mon Mar 24 2025
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 enigma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once

#ifdef _MSC_VER
#include <format>
#include <chrono>
#include <ranges>
#include "string/str.h"

template <>
struct std::formatter<enigma::string, char> {
	template <class ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& ctx) {
		return std::ranges::find(ctx.begin(), ctx.end(), '}');
	}
	template <class FmtContext>
	FmtContext::iterator format(enigma::string s, FmtContext& ctx) const {
		return std::format_to(ctx.out(), "{}", s.c_str());
	}
};

namespace enigma {
	using std::format;
	using std::localtime;
	using std::make_format_args;
	using std::vformat;

	inline auto localtime(const std::time_t t) -> std::chrono::system_clock::time_point {
		return std::chrono::system_clock::from_time_t(t);
	}

}  // namespace enigma
#else 
#include <ranges>
#include "fmt/format.h"
#include "fmt/chrono.h"

struct fmt::formatter<enigma::string> {
	constexpr auto parse(fmt::format_parse_context& ctx) { return std::ranges::find(ctx.begin(), ctx.end(), '}'); }

	auto format(const enigma::string& p, fmt::format_context& context) {
		return fmt::format_to(context.out(), "{}", (std::string)p);
	}
};

namespace enigma {
	using fmt::format;
	using fmt::make_format_args;
	using fmt::vformat;
	using fmt::localtime;
} 
#endif

/*
 * Created on Mon Mar 24 2025
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 ema
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
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include <any>
#include <random>
#include <algorithm>
#include "class.h"
#include "singleton.h"
#include "string/str.h"

namespace std {
	template <>
	struct hash<ema::string> {
		std::size_t operator()(const ema::string& s) const noexcept { return std::hash<std::string>()(s); }
	};
}  // namespace std

namespace ema {
	using i16	= short;
	using u16	= unsigned short;
	using i32	= int;
	using u32	= unsigned int;
	using i32l	= long;
	using u32l	= unsigned long;
	using i64	= long long;
	using u64	= unsigned long long;
	using f32	= float;
	using f64	= double;
	using byte	= char;
	using bytes = char*;
	using any = std::any;

	using std::any_cast;

	template <typename T>
	using vector = std::vector<T>;

	template <typename K, typename V>
	using map = std::map<K, V>;

	template <typename K, typename V>
	using hash_map = std::unordered_map<K, V>;

	template <typename T>
	using set = std::set<T>;

	template <typename T>
	using hash_set = std::unordered_set<T>;

	template <typename T>
	using queue = std::queue<T>;

	template <typename T>
	using shared_ptr = std::shared_ptr<T>;

	template <typename T, typename D = std::default_delete<T>>
	using unique_ptr = std::unique_ptr<T, D>;

	using std::make_shared;
	using std::make_unique;
	using std::move;
	using std::forward;

	template <typename T>
	using func = std::function<T>;

	template <typename T>
	using optional = std::optional<T>;

	using nullopt_t = std::nullopt_t;

	using err = std::pair<bool, string>;

	template<typename T>
	using result = std::pair<bool, optional<T>>;

	inline constexpr nullopt_t nil = nullopt_t{nullopt_t::_Tag{}};


	template<typename T>
	inline T random(const T from, const T to) {
		std::random_device rd;
		std::mt19937 gen(rd());
		return std::uniform_int_distribution<u64>(from, to)(gen);
	}
}  // namespace EMA

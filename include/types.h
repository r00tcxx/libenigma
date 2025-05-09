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
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "string/str.h"
#include "class.h"
#include "singleton.h"

namespace std {
	template <>
	struct hash<enigma::string> {
		std::size_t operator()(const enigma::string& s) const noexcept {
			return std::hash<std::string>()(s);
		}
	};
}  // namespace std

namespace enigma {
	using i16 = short;
	using u16 = unsigned short;
	using i32  = int;
	using u32  = unsigned int;
	using i32l = long;
	using u32l = unsigned long;
	using i64  = long long;
	using u64  = unsigned long long;
	using f32  = float;
	using f64  = double;
	using byte = char;
	using bytes = char*; 

	using result = std::pair<bool, string>;

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
	using sharde_ptr = std::shared_ptr<T>;

	template <typename T, typename D = std::default_delete<T>>
	using unique_ptr = std::unique_ptr<T, D>;

	using std::make_unique;
	using std::make_shared;

	template <typename T>
	using func = std::function<T>;
}  // namespace enigma

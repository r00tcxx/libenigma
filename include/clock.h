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
#include <chrono>
#include "types.h"

namespace ema {
	using microseconds = std::chrono::microseconds;
	using minseconds   = std::chrono::milliseconds;
	using seconds	   = std::chrono::seconds;
	using mins		   = std::chrono::minutes;
	using houers	   = std::chrono::hours;

	template <typename T>
	inline std::size_t now() {
		return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	class clock {
	   public:
		clock() {}
		~clock() {}

		inline void begin() { _start_time = std::chrono::system_clock::now(); }
		inline u64 end() {
			auto end_time = std::chrono::system_clock::now();
			return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - _start_time).count();
		}

	   private:
		std::chrono::time_point<std::chrono::system_clock> _start_time;
	};
}  // namespace ema

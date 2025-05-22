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
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stop_token>
#include <thread>

namespace ema {
	using thread			 = std::thread;
	using jthread			 = std::jthread;
	using stop_source		 = std::stop_source;
	using stop_token		 = std::stop_token;
	using condition_variable = std::condition_variable;
	using mutex				 = std::mutex;
	using shared_mutex		 = std::shared_mutex;
	using atomic_bool		 = std::atomic_bool;
	using atomic_u32		 = std::atomic_uint32_t;
	using atomic_u64		 = std::atomic_uint64_t;

	template <typename T>
	using atomic_shared_ptr = std::atomic<std::shared_ptr<T>>;

	template <typename T>
	using unique_lock = std::unique_lock<T>;

	template <typename T>
	using lock_guard = std::lock_guard<T>;

}  // namespace ema

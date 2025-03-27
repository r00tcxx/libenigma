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
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <stop_token>

namespace enigma {
	template <typename T>
	class sync_queue {
	   public:
		sync_queue() = default;
		sync_queue(const sync_queue& other) { operator=(other); }
		sync_queue(sync_queue&& other) { operator=(std::move(other)); }
		~sync_queue() = default;

		sync_queue& operator=(const sync_queue& other) noexcept {
			if (this == &other) return *this;
			queue_ = other.queue_;
			cv_.notify_all();
			other.stop();
			return *this;
		}

		sync_queue& operator=(sync_queue&& other) noexcept {
			if (this == &other) return *this;
			queue_ = std::move(other.queue_);
			cv_.notify_all();
			other.stop();
			return *this;
		}

		template <typename U>
		void push(U&& value) {
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.push(std::forward<U>(value));
			cv_.notify_one();
		}

		std::optional<T> pop() {
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this] { return !queue_.empty() || stop_source_.stop_requested(); });
			if (stop_source_.stop_requested()) return std::nullopt;
			T value = std::move(queue_.front());
			queue_.pop();
			return value;
		}

		void clear() {
			std::lock_guard<std::mutex> lock(mutex_);
			while (!queue_.empty())
				queue_.pop();
		}

		void stop() {
			stop_source_.request_stop();
			cv_.notify_all();
		}

	   private:
		std::queue<T> queue_;
		std::mutex mutex_;
		std::condition_variable cv_;
		std::stop_source stop_source_;
	};
}  // namespace enigma

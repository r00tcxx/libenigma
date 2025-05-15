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
#include "../sync_queue.h"
#include "../types.h"

namespace ema {
	namespace detail {
		template <typename T>
		concept msg_t = std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;

		struct event {
			string type;
			any ev;
			func<void()> ack{nullptr};
		};

	}  // namespace detail

	class event_bus;
	class event_handler : public std::enable_shared_from_this<event_handler> {
		friend event_bus;

	   public:
		using ptr = sharde_ptr<event_handler>;

	   public:
		event_handler()	 = default;
		~event_handler() = default;

		inline void start() {
			if (_start) return;
			_thread = jthread(
				[this](stop_token stop_token) {
					while (!stop_token.stop_requested()) {
						auto event_opt = _events.pop();
						if (!event_opt.has_value()) continue;
						lock_guard lock(_mutex);
						auto& event = event_opt.value();
						auto it		= _event_handles.find(event.type);
						if (it == _event_handles.end()) continue;
						it->second(event);
					}
				});
			_start = true;
		}

		inline void stop() {
			if (!_start) return;
			_thread.request_stop();
			_events.stop();
			if (_thread.joinable()) _thread.join();
			_events.clear();
			_start = false;
		}

		template <detail::msg_t T>
		inline void on(func<void(const T)>&& callback) {
			if (!callback) return;
			string type_name = typeid(T).name();
			lock_guard lock(_mutex);
			if (auto it = _event_handles.find(type_name); it == _event_handles.end()) {
				_event_handles.emplace(type_name, [callback = std::forward<decltype(callback)>(callback)](detail::event event) {
					try {
						auto target_event = any_cast<T>(event.ev);
						if (callback) callback(target_event);
						if (event.ack) event.ack();
					} catch (...) {}
				});
			}
		}

		template <detail::msg_t T>
		inline void cancel() {
			string type_name = typeid(T).name();
			lock_guard lock(_mutex);
			_event_handles.erase(type_name);
		}

		inline void clear() { _events.clear(); }

		inline bool is_start() { return _start; }

	   private:
		bool _has_type(const string& type_name) {
			lock_guard lock(_mutex);
			return _event_handles.contains(type_name);
		}

		void _deliver(detail::event ev) { _events.push(ev); }

	   private:
		sync_queue<detail::event> _events;
		mutex _mutex;
		hash_map<string, func<void(detail::event)>> _event_handles;
		jthread _thread;
		atomic_bool _start{false};
	};
}  // namespace ema
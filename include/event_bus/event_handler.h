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
#include <any>
#include "../container/sync_queue.h"

namespace ema {
	namespace detail {
		template <typename T>
		concept MSGType = std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;

		struct Event {
			std::string type;
			std::any ev;
			std::function<void()> ack{nullptr};
		};

	}  // namespace detail

	class EventBus;
	class EventHandler : public std::enable_shared_from_this<EventHandler> {
		friend EventBus;

	   public:
		using Ptr = std::shared_ptr<EventHandler>;

	   public:
		EventHandler()	= default;
		~EventHandler() = default;

		inline void Start() {
			if (_start) return;
			_thread = std::jthread([this](std::stop_token stop_token) {
				while (!stop_token.stop_requested()) {
					auto event_opt = _events.Pop();
					if (!event_opt.has_value()) continue;
					std::lock_guard lock(_mutex);
					auto& event = event_opt.value();
					auto it		= _event_handles.find(event.type);
					if (it == _event_handles.end()) continue;
					it->second(event);
				}
			});
			_start	= true;
		}

		inline void Stop() {
			if (!_start) return;
			_thread.request_stop();
			_events.Stop();
			if (_thread.joinable()) _thread.join();
			_events.Clear();
			_start = false;
		}

		template <detail::MSGType T>
		inline bool On(std::function<void(const T&)>&& callback) {
			if (!callback) return false;
			std::string type_name = typeid(T).name();
			std::lock_guard lock(_mutex);
			if (auto it = _event_handles.find(type_name); it == _event_handles.end()) {
				_event_handles.emplace(type_name,
									   [callback = std::forward<decltype(callback)>(callback)](detail::Event event) {
										   try {
											   auto target_event = std::any_cast<T>(event.ev);
											   if (callback) callback(target_event);
											   if (event.ack) event.ack();
										   } catch (...) {}
									   });
				return true;
			}
			return false;
		}

		inline bool OnType(const std::string& type, std::function<void(detail::Event)>&& callback) {
			if (type.empty() || !callback) return false;
			std::lock_guard lock(_mutex);
			if (auto it = _event_handles.find(type); it == _event_handles.end()) {
				_event_handles.emplace(type, callback);
				return true;
			}
			return false;
		}

		template <detail::MSGType T>
		inline void Cancel() {
			std::string type_name = typeid(T).name();
			std::lock_guard lock(_mutex);
			_event_handles.erase(type_name);
		}

		inline void Clear() {
			_events.Clear();
		}

		inline bool IsStart() {
			return _start;
		}

	   private:
		bool _HasType(const std::string& type_name) {
			std::lock_guard lock(_mutex);
			return _event_handles.contains(type_name);
		}

		void _Deliver(detail::Event ev) {
			_events.Push(ev);
		}

	   private:
		SyncQueue<detail::Event> _events;
		std::mutex _mutex;
		std::unordered_map<std::string, std::function<void(detail::Event)>> _event_handles;
		std::jthread _thread;
		std::atomic_bool _start{false};
	};
}  // namespace ema
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
#include <set>
#include "event_handler.h"

namespace ema {
	class EventBus {
	   public:
		EventBus()	= default;
		~EventBus() = default;

		template <detail::MSGType T>
		bool Publish(T&& msg, std::function<void()>&& ack_callback = nullptr) {
			std::string type_name = typeid(T).name();
			std::lock_guard lock(_mutex);
			bool result{false};
			for (auto& handler : _handlers) {
				if (handler->_HasType(type_name)) {
					handler->_Deliver(detail::Event{.type = type_name, .ev = msg, .ack = ack_callback});
					result = true;
				}
			}
			return result;
		}

		bool Publish(const std::string& type_name, std::any&& any, std::function<void()>&& ack_callback = nullptr) {
			std::lock_guard lock(_mutex);
			bool result{false};
			for (auto& handler : _handlers) {
				if (handler->_HasType(type_name)) {
					handler->_Deliver(detail::Event{.type = type_name, .ev = move(any), .ack = ack_callback});
					result = true;
				}
			}
			return result;
		}

		bool Subscribe(EventHandler::Ptr handler) {
			if (!handler) return false;
			std::lock_guard lock(_mutex);
			return _handlers.insert(handler).second;
		}

		void Unsubscribe(EventHandler::Ptr handler) {
			if (!handler) return;
			std::lock_guard lock(_mutex);
			_handlers.erase(handler);
		}

	   private:
		std::mutex _mutex;
		std::set<EventHandler::Ptr> _handlers;
	};
}  // namespace ema

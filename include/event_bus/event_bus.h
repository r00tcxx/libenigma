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
#include "event_handler.h"

namespace ema {
	class event_bus : public no_cmable {
	   public:
		event_bus()	 = default;
		~event_bus() = default;

		template <detail::msg_t T>
		bool publish(T&& msg, func<void()>&& ack_callback = nullptr) {
			string type_name = typeid(T).name();
			lock_guard lock(_mutex);
			bool result{false};
			for (auto& handler : _handlers) {
				if (handler->_has_type(type_name)) {
					handler->_deliver(detail::event{.type = type_name, .ev = msg, .ack = ack_callback});
					result = true;
				}
			}
			return result;
		}

		bool subscribe(event_handler::ptr handler) {
			if (!handler) return false;
			lock_guard lock(_mutex);
			return _handlers.insert(handler).second;
		}

		void unsubscribe(event_handler::ptr handler) {
			if (!handler) return;
			lock_guard lock(_mutex);
			_handlers.erase(handler);
		}

	   private:
		mutex _mutex;
		set<event_handler::ptr> _handlers;
	};
}  // namespace ema

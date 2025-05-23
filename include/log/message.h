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
#include <memory>
#include <string>

namespace ema::log {
	enum class LogLevel : int { Trace, TraceError, Debug, Info, Warn, Error, Fatal };

	class Message {
	   public:
		Message(const LogLevel lvl, const std::size_t time, const std::size_t thread, const char* modlue,
				std::string&& msg) noexcept
			: _lvl(lvl), _time(time), _thread(thread), _module(modlue), _msg(std::move(msg)) {
		}

		Message(const Message&) = delete;

		Message(Message&& other) noexcept {
			operator=(std::move(other));
		}

		~Message() = default;

		Message& operator=(Message&& other) noexcept {
			_lvl	= other._lvl;
			_time	= other._time;
			_thread = other._thread;
			_module = other._module;
			_msg	= std::move(other._msg);
			return *this;
		}

		Message& operator=(const Message&) = delete;

		inline LogLevel Level() const {
			return _lvl;
		}
		inline std::size_t Timestamp() const {
			return _time;
		}
		inline std::size_t Thread() const {
			return _thread;
		}
		inline const std::string& Content() const {
			return _msg;
		}
		inline const char* Module() const {
			return _module;
		}

	   private:
		LogLevel _lvl;
		const char* _module;
		std::size_t _time;
		std::size_t _thread;
		std::string _msg;
	};
}  // namespace ema::log

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
#include "class.h"
#include "types.h"

namespace ema::log {
	enum class log_level : int { trace, trace_error, debug, info, warn, error, fatal };

	class message : public no_cmable {
	   public:
		message(const log_level lvl, const u64 time, const u64 thread, const char* modlue, string&& msg) noexcept
			: _lvl(lvl), _time(time), _thread(thread), _module(modlue), _msg(std::move(msg)) {}
		message(message&& other) noexcept { operator=(std::move(other)); }
		~message() = default;

		message& operator=(message&& other) noexcept {
			_lvl	= other._lvl;
			_time	= other._time;
			_thread = other._thread;
			_module = other._module;
			_msg	= std::move(other._msg);
			return *this;
		}

		inline log_level level() const { return _lvl; }
		inline u64 timestamp() const { return _time; }
		inline u64 thread() const { return _thread; }
		inline const string& content() const { return _msg; }
		inline const char* module() const { return _module; }

	   private:
		log_level _lvl;
		const char* _module;
		u64 _time;
		u64 _thread;
		string _msg;
	};
}  // namespace ema::log

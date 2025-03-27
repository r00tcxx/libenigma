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

namespace enigma::log {
	enum class log_level : int { debug, info, warn, error, fatal };

	using formatter = func<string(const log_level /*lvl*/, const u64 /*time*/, const u64 /*thread*/,
								  const char* /*file*/, const int /*line*/)>;

	class message : public no_cmable {
	   public:
		message(const log_level lvl, const u64 time, const u64 thread, const char* file, const int line,
				string&& msg) noexcept
			: lvl_(lvl), time_(time), thread_(thread), file_(file), line_(line), msg_(std::move(msg)) {}
		message(message&& other) noexcept { operator=(std::move(other)); }
		~message() = default;

		message& operator=(message&& other) noexcept {
			lvl_	= other.lvl_;
			time_	= other.time_;
			thread_ = other.thread_;
			file_	= other.file_;
			line_	= other.line_;
			msg_	= std::move(other.msg_);
			return *this;
		}

		log_level level() const { return lvl_; }
		u64 timestamp() const { return time_; }
		u64 thread() const { return thread_; }
		const string& content() const { return msg_; }
		const char* file() const { return file_; }
		int line() const { return line_; }

	   private:
		log_level lvl_;
		u64 time_;
		u64 thread_;
		string msg_;
		const char* file_;
		int line_;
	};
}  // namespace enigma::log

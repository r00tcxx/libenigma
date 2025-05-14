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
#include "clock.h"
#include "format.h"
#include "native/process.h"
#include "singleton.h"
#include "sink.h"

namespace ema::log {
	bool init_logger(const log_level lvl, std::vector<sink::ptr>&& sinks);
	void uninit_logger();
	void log_it(message&& msg);

	unique_ptr<sink> make_file_sink(file_sink_config&& config);
	unique_ptr<sink> make_console_sink(console_sink_config&& config);

	/*
	* Since we're using the fmt library, 
	* we don't need to worry about memory allocation issues—the fmt library handles it automatically.
	*/
	template <typename... Args>
	void debug(string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::debug, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void debug(const char* file, const int line, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::debug, now<minseconds>(), native::process::get_current_thread_id(), file, line,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void debug_specific(const char* m, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::debug, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0, m,
					   std::move(s)));
	}

	template <typename... Args>
	void info(string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::info, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void info(const char* file, const int line, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::info, now<minseconds>(), native::process::get_current_thread_id(), file, line,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void info_specific(const char* m, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::info, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0, m,
					   std::move(s)));
	}

	template <typename... Args>
	void warn(string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::warn, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void warn(const char* file, const int line, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::warn, now<minseconds>(), native::process::get_current_thread_id(), file, line,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void warn_specific(const char* m, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::warn, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0, m,
					   std::move(s)));
	}

	template <typename... Args>
	void error(string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::error, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void error(const char* file, const int line, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::error, now<minseconds>(), native::process::get_current_thread_id(), file, line,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void error_specific(const char* m, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::error, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0, m,
					   std::move(s)));
	}

	template <typename... Args>
	void fatal(string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::fatal, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void fatal(const char* file, const int line, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::fatal, now<minseconds>(), native::process::get_current_thread_id(), file, line,
					   nullptr, std::move(s)));
	}

	template <typename... Args>
	void fatal_specific(const char* m, string_view format, Args&&... args) {
		auto s = vformat(format, make_format_args(args...));
		log_it(message(log_level::fatal, now<minseconds>(), native::process::get_current_thread_id(), nullptr, 0, m,
					   std::move(s)));
	}

	class specific_logger {
	   public:
		specific_logger(const char* name) : name_(name) {}
		~specific_logger() = default;

		template <typename... Args>
		void debug(string_view format, Args&&... args) {
			log::debug_specific(name_, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void info(string_view format, Args&&... args) {
			log::info_specific(name_, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void warn(string_view format, Args&&... args) {
			log::warn_specific(name_, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void error(string_view format, Args&&... args) {
			log::error_specific(name_, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		void fatal(string_view format, Args&&... args) {
			log::fatal_specific(name_, format, std::forward<Args>(args)...);
		}

	   private:
		const char* name_;
	};
}  // namespace ema::log

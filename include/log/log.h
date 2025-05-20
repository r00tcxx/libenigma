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
#include "sink.h"

#define LOG_DECLARE(Level)                                                                                           \
	template <typename... Args>                                                                                      \
	void Level(string_view format, Args&&... args) {                                                                 \
		auto s = vformat(format, make_format_args(args...));                                                         \
		log_it(message(log_level::##Level, now<seconds>(), native::process::get_current_thread_id(), nullptr,        \
					   std::move(s)));                                                                               \
	}                                                                                                                \
	template <typename... Args>                                                                                      \
	void Level##_specific(const char* m, string_view format, Args&&... args) {                                       \
		auto s = vformat(format, make_format_args(args...));                                                         \
		log_it(                                                                                                      \
			message(log_level::##Level, now<seconds>(), native::process::get_current_thread_id(), m, std::move(s))); \
	}

#define LOG_SPECIFIC(Level)                                                \
	template <typename... Args>                                            \
	void Level(string_view format, Args&&... args) {                       \
		log::Level##_specific(name_, format, std::forward<Args>(args)...); \
	}

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
	LOG_DECLARE(debug);
	LOG_DECLARE(info);
	LOG_DECLARE(warn);
	LOG_DECLARE(error);
	LOG_DECLARE(fatal);
	LOG_DECLARE(trace);
	LOG_DECLARE(trace_error);

	class specific_logger {
	   public:
		specific_logger(const char* name) : name_(name) {}
		~specific_logger() = default;

		LOG_SPECIFIC(debug);
		LOG_SPECIFIC(info);
		LOG_SPECIFIC(warn);
		LOG_SPECIFIC(error);
		LOG_SPECIFIC(fatal);
		LOG_SPECIFIC(trace);
		LOG_SPECIFIC(trace_error);

	   private:
		const char* name_;
	};
}  // namespace ema::log

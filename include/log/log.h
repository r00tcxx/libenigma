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
#include "fmt/format.h"
#include "native/process.h"
#include "singleton.h"
#include "sink.h"

#define LOG_DEBUG(...) enigma::log::debug(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) enigma::log::info(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) enigma::log::warn(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) enigma::log::error(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) enigma::log::fatal(__FILE__, __LINE__, __VA_ARGS__)

namespace enigma::log {
	bool init_logger(const log_level lvl, std::vector<sink::ptr>&& sinks, formatter&& formatter = nullptr);
	void uninit_logger();
	void log_it(message&& msg);

	unique_ptr<sink> make_file_sink(file_sink_config&& config);
	unique_ptr<sink> make_console_sink(console_sink_config&& config);

	/*
	* Since we're using the fmt library, 
	* we don't need to worry about memory allocation issues—the fmt library handles it automatically.
	*/
	template <typename... Args>
	void log_debug(const char* format, Args&&... args) {

		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::debug, time(NULL), native::process::get_current_thread_id(), nullptr, 0, std::move(s)));
	}

	template <typename... Args>
	void log_debug(const char* file, const int line, const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::debug, time(NULL), native::process::get_current_thread_id(), file, line, std::move(s)));
	}

	template <typename... Args>
	void log_info(const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::info, time(NULL), native::process::get_current_thread_id(), nullptr, 0, std::move(s)));
	}

	template <typename... Args>
	void log_info(const char* file, const int line, const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::info, time(NULL), native::process::get_current_thread_id(), file, line, std::move(s)));
	}

	template <typename... Args>
	void log_warn(const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::warn, time(NULL), native::process::get_current_thread_id(), nullptr, 0, std::move(s)));
	}

	template <typename... Args>
	void log_warn(const char* file, const int line, const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::warn, time(NULL), native::process::get_current_thread_id(), file, line, std::move(s)));
	}

	template <typename... Args>
	void log_error(const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::error, time(NULL), native::process::get_current_thread_id(), nullptr, 0, std::move(s)));
	}

	template <typename... Args>
	void log_error(const char* file, const int line, const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::error, time(NULL), native::process::get_current_thread_id(), file, line, std::move(s)));
	}

	template <typename... Args>
	void log_fatal(const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::fatal, time(NULL), native::process::get_current_thread_id(), nullptr, 0, std::move(s)));
	}

	template <typename... Args>
	void log_fatal(const char* file, const int line, const char* format, Args&&... args) {
		auto s = fmt::vformat(format, fmt::make_format_args(std::forward<Args>(args)...));
		log_it(
			message(log_level::fatal, time(NULL), native::process::get_current_thread_id(), file, line, std::move(s)));
	}
}  // namespace enigma::log

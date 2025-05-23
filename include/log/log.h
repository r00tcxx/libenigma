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
#include <string_view>
#include "format.h"
#include "native/process.h"
#include "sink.h"
#include "time/clock.h"

#define LOG_DECLARE(Level)                                                                                            \
	template <typename... Args>                                                                                       \
	void Level(std::string_view format, Args&&... args) {                                                             \
		auto s = vformat(format, make_format_args(args...));                                                          \
		LogIt(Message(LogLevel::##Level, ema::Now<Seconds>(), native::process::GetCurrentThreadID(), nullptr,         \
					  std::move(s)));                                                                                 \
	}                                                                                                                 \
	template <typename... Args>                                                                                       \
	void Level##_specific(const char* m, std::string_view format, Args&&... args) {                                   \
		auto s = vformat(format, make_format_args(args...));                                                          \
		LogIt(                                                                                                        \
			Message(LogLevel::##Level, ema::Now<Seconds>(), native::process::GetCurrentThreadID(), m, std::move(s))); \
	}

#define LOG_SPECIFIC(Level)                                                \
	template <typename... Args>                                            \
	void Level(string_view format, Args&&... args) {                       \
		log::Level##_specific(name_, format, std::forward<Args>(args)...); \
	}

namespace ema::log {
	bool InitLogger(const LogLevel lvl, std::vector<Sink::Ptr>&& sinks);
	void InitLogger();
	void LogIt(Message&& msg);

	std::unique_ptr<Sink> MakeFileSink(FileSinkConfig&& config);
	std::unique_ptr<Sink> MakeConsoleSink(ConsoleSinkConfig&& config);

	/*
	* Since we're using the fmt library, 
	* we don't need to worry about memory allocation issues—the fmt library handles it automatically.
	*/
	LOG_DECLARE(Debug);
	LOG_DECLARE(Info);
	LOG_DECLARE(Warn);
	LOG_DECLARE(Error);
	LOG_DECLARE(Fatal);
	LOG_DECLARE(Trace);
	LOG_DECLARE(TraceError);

	class SpecificLogger {
	   public:
		SpecificLogger(const char* name) : name_(name) {
		}
		~SpecificLogger() = default;

		LOG_SPECIFIC(Debug);
		LOG_SPECIFIC(Info);
		LOG_SPECIFIC(Warn);
		LOG_SPECIFIC(Error);
		LOG_SPECIFIC(Fatal);
		LOG_SPECIFIC(Trace);
		LOG_SPECIFIC(TraceError);

	   private:
		const char* name_;
	};
}  // namespace ema::log

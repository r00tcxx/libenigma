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
#include "message.h"

namespace ema::log {
	class Sink {
	   public:
		using Ptr = std::unique_ptr<Sink>;

	   public:
		Sink()						 = default;
		Sink(const Sink&)			 = delete;
		Sink(Sink&&)				 = delete;
		Sink& operator=(const Sink&) = delete;
		Sink& operator=(Sink&&)		 = delete;

	   public:
		virtual ~Sink()											 = default;
		virtual bool Init()										 = 0;
		virtual void Uninit()									 = 0;
		virtual bool Log(const LogLevel lvl, const Message& msg) = 0;
	};

	struct FileSinkConfig {
		enum class Mode { Append, Truncate };
		std::string log_dir;
		std::string app_name;
		std::size_t max_file_size{0};
		unsigned int max_file_count{1};
		Mode mode{Mode::Append};
	};

	struct ConsoleSinkConfig {
		bool color{true};
	};
}  // namespace ema::log

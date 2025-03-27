#pragma once
#include <chrono>
#include <string>
#include "fmt/chrono.h"
#include "fmt/format.h"
#include "log/message.h"

namespace enigma::log {
	class default_formatter {
	   public:
		default_formatter()	 = default;
		~default_formatter() = default;

		std::string operator()(const log_level lvl, const u64 time, const u64 thread, const char* /*file*/,
							   const int /*line*/) {
			std::string lvl_str;
			switch (lvl) {
				case log_level::debug:
					lvl_str = "DEBUG";
					break;
				case log_level::info:
					lvl_str = "INFO";
					break;
				case log_level::warn:
					lvl_str = "WARN";
					break;
				case log_level::error:
					lvl_str = "ERROR";
					break;
				case log_level::fatal:
					lvl_str = "FATAL";
					break;
			}
			return fmt::format("{} [{}] {}>", fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(time)), lvl_str,
							   thread);
		}
	};

}  // namespace enigma::log
